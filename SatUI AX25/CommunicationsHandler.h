/*
   Author: AlvyneZ

   Code for NaSPUoN Communications handling
*/

#ifndef COMMS_HANDLER_H
#define COMMS_HANDLER_H


#include <vector>
#include <map>
#include <fstream>
#include <string>

#include "Logger.h"
#include "fileOperations.h"
#include "AX25Communication.h"
#include "bitManipulation.h"
#include "fileTransferClass.h"


#define HK_LOG_FILE "hk_data.txt"
#define SAT_CONFIG_FILE_NAME "sat_settings.cfg"


/*-------------------Packets Identifications---------------

	Data Transfer:
	  Transfer Request				-> TR
	  Transfer Initializer			-> TI
	  Begin Transfer				-> TB
	  Transfer Data					-> TD
	  Transfer Complete				-> TC

	Files Exploration:
	  Images Taken					-> IM
	  LoRa Packets Received			-> LR
	  Old Log Files					-> LF

	HouseKeeping
	  Housekeeping Data request		-> HK
	  Log File Move					-> LM

	Date/Time:
	  Request Date and Time			-> DR
	  Save Date and Time			-> DS

*/







void SatUI::MyForm::addPacket(FileTransfer * ft, std::vector<uint8_t> & packet) {
	if (packet.size() < 8) //To avoid accessing outside vector size
		return;
	if ((packet[0] == 'T') && (packet[1] == 'D')) {
		uint16_t tID = getSixteenBitIntFromEightBitVector(packet, 2);
		if (tID == ft->transferID) {
			uint32_t pckIndex = getThirtyTwoBitIntFromEightBitVector(packet, 4);
			packet.erase(packet.begin(), packet.begin() + 8);
			if (pckIndex < ft->expectedPackets) {
				ft->packets[pckIndex] = packet;
				if (!ft->receivedPacket[pckIndex]) {
					ft->packetsReceived++;
					adjuplinkProgressBarUpdate((ft->packetsReceived * 100) / ft->expectedPackets);
				}
				ft->receivedPacket[pckIndex] = true;
			}
			else {
				logErr("CommHndl -> Packet index larger or equal to expected packets. tID:" + std::to_string(tID));
			}
		}
		else {
			std::string tIDErr = "Expected " + std::to_string(ft->transferID) + " but got packet with " + std::to_string(tID);
			logErr("CommHndl -> Attempt to add a packet to a wrong filetransfer object {tID doesn't match}. " + tIDErr);
		}
	}
	else {
		std::string start = " Packet started with '";
		start += packet[0];
		start += packet[1];
		start += "'.";
		logErr("CommHndl -> Attempt to add a packet that doesn't begin with 'TD'." + start + " tID:" + std::to_string(ft->transferID));
	}
}

std::vector<uint8_t> SatUI::MyForm::getInitializationPacket(FileTransfer * ft) {
	return ft->packets[0];
}

std::vector<uint8_t> SatUI::MyForm::getPacket(FileTransfer * ft, uint32_t index) {
	if (index < ft->expectedPackets) {
		return ft->packets[index + 1];
	}
	else {
		std::string err = "PckIndex requested: " + std::to_string(index) + ". PacketCount: " + std::to_string(ft->expectedPackets) + ".";
		logErr("CommHndl -> Invalid Packet request for tID " + std::to_string(ft->transferID) + ". " + err);
		std::vector<uint8_t> ret;
		return ret;
	}
}

bool SatUI::MyForm::transferComplete(uint16_t tID, std::vector<uint8_t> AX25GSCallsignSSID) {
	///log("Transfer Complete Called with: rcv=" + std::to_string(ft->packetsReceived) + " and expctd=" + std::to_string(ft->expectedPackets));
	if ((CommsNaSPUoN::incomingTransfers[tID].packetsReceived) >= (CommsNaSPUoN::incomingTransfers[tID].expectedPackets)) {
		std::string outputPath = getUplinkSaveLocation();
		outputPath += '\\';
		//CommsNaSPUoN::incomingTransfers[tID].fileName[2] = '\\'; //Since the sat(Linux) uses forward-slashes but windows uses back-slashes
		outputPath += CommsNaSPUoN::incomingTransfers[tID].fileName;
		compilePackets(outputPath, CommsNaSPUoN::incomingTransfers[tID].packets);
		adjuplinkProgressBarUpdate(100);
		return true;
	}
	else {
		if (backgroundWorker_UplinkPartRequest->IsBusy) {
			backgroundWorker_UplinkPartRequest->CancelAsync();
			log("CommHndl -> Cancelled uplink part request thread to allow for a new uplink part request transfer.");
			//Stalling to allow the thread to stop
			System::Threading::Thread::CurrentThread->Sleep(100);
		}
		array<uint8_t>^ args = gcnew array<uint8_t>(9);
		args[1] = tID & 0x00ff;
		args[0] = tID >> 8;
		int index = 0;
		for (; index < AX25GSCallsignSSID.size(); index++) {
			args[2 + index] = AX25GSCallsignSSID[index];
		}
		for (; index < 7; index++) {
			args[2 + index] = ';';
		}

		startBackgroundWorkerUplinkPartRequest(args);

		return false;
	}
}

FileTransfer SatUI::MyForm::initializeFileTransfer(bool incoming, std::vector<uint8_t> & initializerPacket) {
	FileTransfer ft;
	ft.ready = false;
	if (initializerPacket.size() < 4) //To avoid accessing outside vector size
		return ft;
	if (incoming) {
		if ((initializerPacket[0] == 'T') && (initializerPacket[1] == 'I')) {
			if (initializerPacket.size() < 12) //To avoid accessing outside vector size
				return ft;
			ft.transferID = getSixteenBitIntFromEightBitVector(initializerPacket, 2);
			ft.packetsReceived = 0x00000000;
			ft.fileSize = getThirtyTwoBitIntFromEightBitVector(initializerPacket, 4);
			ft.expectedPackets = getThirtyTwoBitIntFromEightBitVector(initializerPacket, 8);

			ft.packets.resize(ft.expectedPackets);
			ft.receivedPacket.resize(ft.expectedPackets, false);

			ft.fileName = "";
			for (unsigned int i = 12; i < initializerPacket.size(); i++)
				ft.fileName += initializerPacket[i];
			if (ft.fileName.size() > 0)
				ft.ready = true;
			else
				logErr("CommHndl -> received a transfer without a fileName.");
		}
		else {
			std::string start = " Packet started with: ";
			start += initializerPacket[0];
			start += initializerPacket[1];
			logErr("CommHndl -> Attempt to start an incoming file transfer with a packet not starting with 'TI'." + start);
		}
	}
	else {
		if ((initializerPacket[0] == 'T') && (initializerPacket[1] == 'R')) {
			msclr::lock lck(this->transferIDMutex);
			ft.transferID = this->transferIDCount;
			(this->transferIDCount)++;
			lck.release();

			ft.fileName = getDownlinkSatFilesLocation();
			ft.fileName = "";
			for (unsigned int i = 3; i < initializerPacket.size(); i++)
				ft.fileName += initializerPacket[i];

			if (ft.fileName[2] == '/') {
				std::string location = "";
				if ( ((ft.fileName[0] == 'l') && (ft.fileName[1] == 'r')) ||
					((ft.fileName[0] == 'l') && (ft.fileName[1] == 'f')) ||
					((ft.fileName[0] == 'i') && (ft.fileName[1] == 'm')) ||
					((ft.fileName[0] == 'h') && (ft.fileName[1] == 'k'))) {
					location = getDownlinkSatFilesLocation() + "\\";

					std::string fileWithPath = ft.fileName;
					fileWithPath[2] = '\\';
					fileWithPath.insert(fileWithPath.begin(), location.begin(), location.end());

					if (checkIfFileExists(fileWithPath)) {
						std::vector<uint8_t> file = retrieveFileToTransfer(fileWithPath);
						ft.fileSize = file.size();

						ft.packets = transferOutPacketsConstruction(ft.transferID, ft.fileName, file);
						ft.expectedPackets = ft.packets.size() - 1;

						ft.ready = true;
					}
					else {
						logErr("CommHndl -> Request to transfer out '" + ft.fileName + "' denied since file cannot be found.");
					}
				}
				else {
					logErr("CommHndl -> Request to transfer out '" + ft.fileName + "' denied. File must have lr/, lf/, im/ or hk/prefix.");
				}
			}
			else {
				logErr("CommHndl -> Request to transfer out '" + ft.fileName + "' denied. File must have lr/, lf/, im/ or hk/ prefix.");
			}
		}
		else {
			std::string start = " Packet started with: ";
			start += initializerPacket[0];
			start += initializerPacket[1];
			logErr("CommHndl -> Attempt to start an outgoing file transfer with a packet not starting with 'TR'." + start);
		}
	}
	return ft;
}


/*void GSUI::MyForm::allowUplinkTransferToContinue(uint8_t frameID) {
	//This function is called when a transmit response is received so that it can let
	//  the uplink thread know that it can send the next rf packet
	msclr::lock lck(this->uplinkSendNextMutex);
	if (CommsNaSPUoN::uplinkSendNext.count(frameID)) {
		CommsNaSPUoN::uplinkSendNext.erase(frameID);
	}
	lck.release();
}*/


void SatUI::MyForm::sendRFPacket(std::vector<uint8_t> AX25GSCallsignSSID, std::vector<uint8_t> & packet) {
	//This function sends an RF packet with either or both the XBee and/or StenSat
	if (packet.size() > 1) {
		sendRFPacketAX25(AX25GSCallsignSSID, packet);
	}
	else {
		logErr("CommHndl -> Attempt to send an Rf packet of size: " + std::to_string(packet.size()) + " denied.");
	}
}

void SatUI::MyForm::downlinkTransfer(std::vector<uint8_t> AX25GSCallsignSSID, uint16_t tID, System::ComponentModel::DoWorkEventArgs^  e) {
	log("CommHndl -> Beginning outgoing transfer of tID " + std::to_string(tID) + ".");
	std::vector<uint8_t> pck;
	uint32_t expectedPackets = CommsNaSPUoN::outgoingTransfers[tID].expectedPackets;
	uint8_t lastFrameID;

	array<uint16_t>^ args = gcnew array<uint16_t>(2);
	args[1] = tID;

	for (uint32_t i = 0; i < expectedPackets; i++) {
		pck = getPacket(&(CommsNaSPUoN::outgoingTransfers[tID]), i);
		sendRFPacket(AX25GSCallsignSSID, pck);

		while (KISS::awaitTNC || (KISS::kissOutBuffer.size() > KISS_OUT_BUFFER_LIMIT)) {
			if (this->backgroundWorker_Downlink[tID]->CancellationPending) {
				e->Cancel = true;
				e->Result = tID;
				return;
			}
		}

		args[0] = (i * 100) / expectedPackets;
		adjdownlinkProgressBarUpdate(args);
	}

	pck.clear();
	pck.push_back('T');
	pck.push_back('C');
	insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);
	pck.push_back(0x01);
	sendRFPacket(AX25GSCallsignSSID, pck);

	args[0] = 100;
	adjdownlinkProgressBarUpdate(args);
}

void SatUI::MyForm::uplinkPartRequestTransfer(std::vector<uint8_t> AX25GSCallsignSSID, uint16_t tID, System::ComponentModel::DoWorkEventArgs^  e) {

	uint32_t expectedPackets = CommsNaSPUoN::incomingTransfers[tID].expectedPackets;
	std::vector<uint8_t> pck;
	uint8_t lastFrameID;
	for (uint32_t i = 0; i < expectedPackets; i++) {
		if (this->backgroundWorker_UplinkPartRequest->CancellationPending) {
			e->Cancel = true;
			return;
		}
		if (!CommsNaSPUoN::incomingTransfers[tID].receivedPacket[i]) {
			pck.clear();
			pck.push_back('T');
			pck.push_back('R');
			pck.push_back(0x0F);
			insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);
			insertThirtyTwoBitIntInEightBitVector(pck, pck.end(), i);
			sendRFPacket(AX25GSCallsignSSID, pck);

			while (KISS::awaitTNC || (KISS::kissOutBuffer.size() > KISS_OUT_BUFFER_LIMIT)) {
				if (this->backgroundWorker_UplinkPartRequest->CancellationPending) {
					e->Cancel = true;
					return;
				}
			}
		}
	}
	pck.clear();
	pck.push_back('T');
	pck.push_back('C');
	insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);
	pck.push_back(0x02);
	sendRFPacket(AX25GSCallsignSSID, pck);
}

void SatUI::MyForm::processIncomingPayload(std::vector<uint8_t> AX25GSCallsignSSID, std::vector<uint8_t> & payload) {
	try {
		//To prevent the receive thread closing because of an error in processing a payload
		if (payload.size() < 2) //To avoid accessing outside vector size
			return;
		if (payload[0] == 'T') {
			if (payload[1] == 'R') {
				//Request to transfer a file or part of a file
				if (payload.size() < 3) //To avoid accessing outside vector size
					return;
				if (payload[2] == 0x00) {
					//Transfer full file
					FileTransfer newFileTransfer = initializeFileTransfer(false, payload);

					if (newFileTransfer.ready) {
						CommsNaSPUoN::outgoingTransfers[newFileTransfer.transferID] = newFileTransfer;
						sendRFPacket(AX25GSCallsignSSID, getInitializationPacket(&newFileTransfer));
						log("CommHndl -> Transfer request received and accepted.");
					}
				}
				else if (payload[2] == 0x03) {
					//Count of packets that were successfully received
					uint16_t tID = getSixteenBitIntFromEightBitVector(payload, 3);

					if (CommsNaSPUoN::outgoingTransfers.count(tID)) {
						uint32_t received = getThirtyTwoBitIntFromEightBitVector(payload, 5);
						array<uint16_t>^ args = gcnew array<uint16_t>(2);
						args[0] = (received * 100) / CommsNaSPUoN::outgoingTransfers[tID].expectedPackets;
						args[1] = tID;
						adjdownlinkProgressBarUpdate(args);
					}
					else {
						logErr("CommHndl -> Invalid progress report for transfer for non-existent tID " + std::to_string(tID) + ".");
					}
				}
				else if (payload[2] == 0x0F) {
					//Transfer part of a file
					if (payload.size() < 9) //To avoid accessing outside vector size
						return;
					uint16_t tID = getSixteenBitIntFromEightBitVector(payload, 3);

					if (CommsNaSPUoN::outgoingTransfers.count(tID)) {
						uint32_t pckIndex = getThirtyTwoBitIntFromEightBitVector(payload, 5);
						sendRFPacket(AX25GSCallsignSSID, getPacket(&(CommsNaSPUoN::outgoingTransfers[tID]), pckIndex));
					}
					else {
						logErr("CommHndl -> Invalid Request for a packet transfer for non-existent tID " + std::to_string(tID) + ".");
					}
				}
				else {
					logErr("CommHndl -> Invalid Request for a packet transfer, wrong bit options: " + std::to_string(payload[2]) + ".");
				}
			}
			else if (payload[1] == 'I') {
				//Transfer Initialization
				if (payload.size() < 4) //To avoid accessing outside vector size
					return;
				uint16_t tID = getSixteenBitIntFromEightBitVector(payload, 2);
				if (CommsNaSPUoN::incomingTransfers.count(tID)) {
					logErr("CommHndl -> Received duplicate initializer for already ongoing incoming transfer " + std::to_string(tID));
					return;
				}
				FileTransfer newFileTransfer = initializeFileTransfer(true, payload);

				std::vector<uint8_t> pck;

				pck.push_back('T');
				pck.push_back('B');

				insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);

				if (newFileTransfer.ready) {
					pck.push_back(0x00);
					CommsNaSPUoN::incomingTransfers[tID] = newFileTransfer;
					currentDownlinkTransferID = tID;
					std::string expPcks = " Expecting " + std::to_string(newFileTransfer.expectedPackets) + " packets.";
					log("CommHndl -> Intitializer packet for uplink transfer tID: " + std::to_string(tID) + " accepted." + expPcks);
					uplinkStarting(tID);
				}
				else {
					pck.push_back(0x01);
					log("CommHndl -> Intitializer packet for uplink transfer tID: " + std::to_string(tID) + " rejected.");
				}

				sendRFPacket(AX25GSCallsignSSID, pck);
			}
			else if (payload[1] == 'B') {
				//Request to begin transfering packets of a file
				if (payload.size() < 5) //To avoid accessing outside vector size
					return;
				uint16_t tID = getSixteenBitIntFromEightBitVector(payload, 2);
				if (payload[4] == 0x00) {
					if ((backgroundWorker_Downlink->count(tID)) && (backgroundWorker_Downlink[tID]->IsBusy)) {
						backgroundWorker_Downlink[tID]->CancelAsync();
						log("CommHndl -> Cancelled downlink thread to allow for a new downlink transfer.");
						//Stalling to allow the thread to stop
						System::Threading::Thread::CurrentThread->Sleep(100);
					}
					array<uint8_t>^ args = gcnew array<uint8_t>(9);
					args[1] = tID & 0x00ff;
					args[0] = tID >> 8;
					int index = 0;
					for (; index < AX25GSCallsignSSID.size(); index++) {
						args[2 + index] = AX25GSCallsignSSID[index];
					}
					for (; index < 7; index++) {
						args[2 + index] = ';';
					}

					startBackgroundWorkerDownlink(args);
				}
				else {
					if (CommsNaSPUoN::outgoingTransfers.count(tID))
						CommsNaSPUoN::outgoingTransfers.erase(tID);
					logErr("CommHndl -> Recepient refused transfer of tID " + std::to_string(tID) + ".");
				}
			}
			else if (payload[1] == 'D') {
				//Incoming transfer of Packets
				if (payload.size() < 4) //To avoid accessing outside vector size
					return;
				uint16_t tID = getSixteenBitIntFromEightBitVector(payload, 2);
				if (CommsNaSPUoN::incomingTransfers.count(tID)) {
					addPacket(&(CommsNaSPUoN::incomingTransfers[tID]), payload);
				}
				else {
					logErr("CommHndl -> Received a data packet for a non-existent tID " + std::to_string(tID) + ".");
				}
			}
			else if (payload[1] == 'C') {
				//Transfer Complete
				if (payload.size() < 5) //To avoid accessing outside vector size
					return;
				uint16_t tID = getSixteenBitIntFromEightBitVector(payload, 2);
				if ((payload[4] == 0x01) && (CommsNaSPUoN::incomingTransfers.count(tID))) {
					bool complete = transferComplete(tID, AX25GSCallsignSSID);
					if (complete) {
						CommsNaSPUoN::incomingTransfers.erase(tID);
						adjuplinkComplete(true);
						std::vector<uint8_t> pck;
						pck.push_back('T');
						pck.push_back('C');
						insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);
						pck.push_back(0x00);
						sendRFPacket(AX25GSCallsignSSID, pck);
						log("CommHndl -> Successfully Completed incoming transfer of tID " + std::to_string(tID) + ".");
					}
				}
				else if ((payload[4] == 0x00) && (CommsNaSPUoN::outgoingTransfers.count(tID))) {
					CommsNaSPUoN::outgoingTransfers.erase(tID);
					adjdownlinkComplete(tID);
					log("CommHndl -> Successfully Completed outgoing transfer of tID " + std::to_string(tID) + ".");
				}
				else if ((payload[4] == 0x02) && (CommsNaSPUoN::outgoingTransfers.count(tID))) {
					std::vector<uint8_t> pck;
					pck.push_back('T');
					pck.push_back('C');
					insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);
					pck.push_back(0x01);
					sendRFPacket(AX25GSCallsignSSID, pck);
				}
				else {
					std::string outOrIn = (payload[4] == 0x01) ? "Incoming" : ((payload[4] == 0x00) || (payload[4] == 0x02)) ? "Outgoing" : "[Direction Unknown]";
					logErr("CommHndl -> Received a transfer complete for a non-existent " + outOrIn + " tID " + std::to_string(tID) + ".");
				}
			}
			else if (payload[1] == 'Z') {
				//Cancel Transfer
				if (payload.size() < 3) //To avoid accessing outside vector size
					return;
				if (payload[2] == 0xFF) {
					//Cancel all incomplete transfers
					cancelAllTransfers(AX25GSCallsignSSID);
				}
				else if (payload[2] == 0x00) {
					//Cancel a specific incomplete transfer
					if (payload.size() < 6) //To avoid accessing outside vector size
						return;
					uint16_t tID = getSixteenBitIntFromEightBitVector(payload, 4);
					if (payload[3] == 0x01) {
						//incoming uplink transfer
						try {
							if (cancelIncomingTransfer(tID)) {
								payload.push_back(0x00);
							}
							else {
								payload.push_back(0x01);
							}
							sendRFPacket(AX25GSCallsignSSID, payload);
						}
						catch (...) {
							payload.push_back(0x0F);
							sendRFPacket(AX25GSCallsignSSID, payload);
							log("CommHndl -> An error occurred when attempting to delete incoming incomplete transfer {" + std::to_string(tID) + "}.");
						}
					}
					else if (payload[3] == 0x00) {
						//outgoing downlink tansfer
						try {
							if (cancelOutgoingTransfer(tID)) {
								payload.push_back(0x00);
							}
							else {
								payload.push_back(0x01);
							}
							sendRFPacket(AX25GSCallsignSSID, payload);
						}
						catch (...) {
							payload.push_back(0x0F);
							sendRFPacket(AX25GSCallsignSSID, payload);
							log("CommHndl -> An error occurred when attempting to delete outgoing incomplete transfer {" + std::to_string(tID) + "}.");
						}
					}
				}
			}
		}
		else if (payload[0] == 'L') {
			if (payload[1] == 'M') {
				//Payload for saving current log file to old log files folder
				payload.push_back(0x00);
				//payload[2] = logFileSave();
				logFileSave();
				sendRFPacket(AX25GSCallsignSSID, payload);
			}
			else if ((payload[1] == 'R') || (payload[1] == 'F')) {
				//Payload for sending file names of lora files or old log files
				bool lora = (payload[1] == 'R');
				std::vector<std::string> files;

				WIN32_FIND_DATA data;
				System::String^ fld;
				if (lora) fld = L"\\lr\\*";
				else fld = L"\\lf\\*";
				System::String^ location = this->downlinkSatFilesLocation + fld;
				pin_ptr<const wchar_t> wname = PtrToStringChars(location);
				HANDLE hFind = FindFirstFile(wname, &data);      // DIRECTORY

				if (hFind != INVALID_HANDLE_VALUE) {
					do {
						std::wstring ws = data.cFileName;
						std::string s(ws.begin(), ws.end());
						if ((s != ".") && (s != "..") && (s != ""))
							files.push_back(s);
					} while (FindNextFile(hFind, &data));
					FindClose(hFind);

					std::vector<uint8_t> pck;
					pck.push_back('L');
					pck.push_back(lora ? 'R' : 'F');
					pck.push_back(0x00);
					pck.push_back(files.size());
					sendRFPacket(AX25GSCallsignSSID, pck);
					for (int i = 0; i < files.size(); i++) {
						pck.clear();
						pck.push_back('L');
						pck.push_back(lora ? 'R' : 'F');
						pck.push_back(0x0F);
						pck.insert(pck.end(), files[i].begin(), files[i].end());
						sendRFPacket(AX25GSCallsignSSID, pck);
					}
				}
				else {
					//Error Opening directory
					std::vector<uint8_t> pck;
					pck.push_back('L');
					pck.push_back(lora ? 'R' : 'F');
					pck.push_back(0x01);
					sendRFPacket(AX25GSCallsignSSID, pck);
					std::string lrf = ((lora) ? "lora" : "log");
					logErr("CommHndl -> Error encountered opening the directory of " + lrf + " files.");
				}
			}
		}
		else if ((payload[0] == 'I') && (payload[1] == 'M')) {
			//Payload for sending file names of images present on the satellite
			std::vector<std::string> files;

			WIN32_FIND_DATA data;
			System::String^ fld = L"\\im\\*";
			System::String^ location = this->downlinkSatFilesLocation + fld;
			pin_ptr<const wchar_t> wname = PtrToStringChars(location);
			HANDLE hFind = FindFirstFile(wname, &data);      // DIRECTORY

			if (hFind != INVALID_HANDLE_VALUE) {
				do {
					std::wstring ws = data.cFileName;
					std::string s(ws.begin(), ws.end());
					if ((s != ".") && (s != "..") && (s != ""))
						files.push_back(s);
				} while (FindNextFile(hFind, &data));
				FindClose(hFind);

				std::vector<uint8_t> pck;
				pck.push_back('I');
				pck.push_back('M');
				pck.push_back(0x00);
				pck.push_back(files.size());
				sendRFPacket(AX25GSCallsignSSID, pck);
				for (int i = 0; i < files.size(); i++) {
					pck.clear();
					pck.push_back('I');
					pck.push_back('M');
					pck.push_back(0x0F);
					pck.insert(pck.end(), files[i].begin(), files[i].end());
					sendRFPacket(AX25GSCallsignSSID, pck);
				}
			}
			else {
				//Error Opening directory
				std::vector<uint8_t> pck;
				pck.push_back('I');
				pck.push_back('M');
				pck.push_back(0x01);
				sendRFPacket(AX25GSCallsignSSID, pck);
				logErr("CommHndl -> Error encountered opening the directory of images.");
			}
		}
		else if ((payload[0] == 'H') && (payload[1] == 'K')) {
			log("CommHndl -> Request to transmit HK data received.");
			std::string file;
			file = "hk/";
			file += HK_LOG_FILE;

			std::vector<uint8_t> pck;
			pck.push_back('T');
			pck.push_back('R');
			pck.push_back(0x00);
			pck.insert(pck.end(), file.begin(), file.end());

			FileTransfer newFileTransfer = initializeFileTransfer(false, payload);

			if (newFileTransfer.ready) {
				CommsNaSPUoN::outgoingTransfers[newFileTransfer.transferID] = newFileTransfer;
				pck.clear();
				pck = getInitializationPacket(&newFileTransfer);
				sendRFPacket(AX25GSCallsignSSID, pck);
				log("CommHndl -> Sent initializer for downlink transfer of tID {" + std::to_string(newFileTransfer.transferID) + "}.");
			}
		}
		else if ((payload[0] == 'X') && (payload[1] == 'C')) {
			//Payload for saving uploaded Satellite Config file
			if (payload.size() < 3) //To avoid accessing outside vector size
				return;
			std::string source = getUplinkSaveLocation();
			source += "\\";
			source += SAT_CONFIG_FILE_NAME;
			std::string configFile = getDownlinkSatFilesLocation();
			configFile += "\\cfg\\";
			configFile += SAT_CONFIG_FILE_NAME;
			if (checkIfFileExists(source)) {
				int result = rename(source.c_str(), configFile.c_str());
				if (result) {
					logErr("CommHndl -> Attempt to copy config file from uplink location failed due to error.");
					payload[2] = 0x02;
				}
				else {
					if (payload[2]) {
						log("CommHndl -> New config file copied from uplink location. Applying the new settings.");
						/*if (setupSatellite()) payload[2] = 0x10;
						else {
							logErr("CommHndl -> Attempt to apply new settings from new config file failed.");
							payload[2] = 0x04;
						}*/
					}
				}
			}
			else {
				logErr("CommHndl -> Attempt to copy config file from uplink location failed. File not found.");
				payload[2] = 0x01;
			}
			sendRFPacket(AX25GSCallsignSSID, payload);
		}
		else if ((payload[0] == 'D') && (payload[1] == 'S')) {
			//Payload for saving date and time to the system date and time
			if (payload.size() < 9) //To avoid accessing outside vector size
				return;
			std::string command;
			command = "sudo date +%Y%m%d%T -s '";

			//Year
			uint16_t year = (payload[2] * 256) + payload[3];
			if (year < 1000) command += "0";
			if (year < 100) command += "0";
			if (year < 10) command += "0";
			command += std::to_string(year);
			//Month
			if (payload[4] < 10) command += "0";
			if ((payload[4] > 12) || (payload[4] == 0)) return;
			command += std::to_string(payload[4]);
			//Day of the month
			if (payload[5] < 10) command += "0";
			if ((payload[5] > 31) || (payload[5] == 0)) return;
			command += std::to_string(payload[5]);

			//Separator between date and time
			command += " ";

			//Hour
			if (payload[6] < 10) command += "0";
			else if (payload[6] > 23) return;
			command += std::to_string(payload[6]);
			command += ":";
			//Minutes
			if (payload[7] < 10) command += "0";
			else if (payload[7] > 59) return;
			command += std::to_string(payload[7]);
			command += ":";
			//Seconds
			if (payload[8] < 10) command += "0";
			else if (payload[8] > 59) return;
			command += std::to_string(payload[8]);

			command += "'";

			//system(command.c_str());
			log("CommHndl -> Time to be saved using the command: " + command);
		}
	}
	catch (...) {
		logErr("CommHndl -> Error processing incoming payload: " + vectorToHexString(payload) + ".");
	}
}


std::vector< std::vector<uint8_t> > SatUI::MyForm::transferOutPacketsConstruction(uint16_t tID, std::string fileName, std::vector<uint8_t> & file) {
	//This function splits a file into packets and adds headers to the packets and adds an extra transfer initialization packet
	//The initialization packet relates the transferId to the filename and should be sent and
	//	acknowledged before all other packets begin to be sent
	//The function adds the transferId and packet count to the beginning of the packets

	uint32_t fileSize = file.size();
	std::vector< std::vector<uint8_t> > packets = splitFileIntoPackets(file);

	std::vector<uint8_t> pck;
	uint32_t packetCount = (uint32_t)(packets.size());

	for (uint32_t i = 0; i < packetCount; i++) {
		pck.clear();
		pck.push_back('T');
		pck.push_back('D');

		insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);

		insertThirtyTwoBitIntInEightBitVector(pck, pck.end(), i);

		packets[i].insert(packets[i].begin(), pck.begin(), pck.end());
	}

	pck.clear();
	pck.push_back('T');
	pck.push_back('I');
	insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);
	insertThirtyTwoBitIntInEightBitVector(pck, pck.end(), fileSize);
	insertThirtyTwoBitIntInEightBitVector(pck, pck.end(), packetCount);
	pck.insert(pck.end(), fileName.begin(), fileName.end());

	packets.insert(packets.begin(), pck);

	return packets;
}

bool SatUI::MyForm::cancelIncomingTransfer(uint16_t tID) {
	if (this->backgroundWorker_UplinkPartRequest->IsBusy) {
		backgroundWorker_UplinkPartRequest->CancelAsync();
		System::Threading::Thread::CurrentThread->Sleep(100); //Stall for backgroundWorker_DownlinkPartRequest to stop
	}
	if (CommsNaSPUoN::incomingTransfers.count(tID)) {
		CommsNaSPUoN::incomingTransfers.erase(tID);
		log("CommHndl -> Incomplete incoming transfer {" + std::to_string(tID) + "} has been deleted successfully.");
		return true;
	}
	else {
		log("The current incoming File Transfer appears to have already completed.");
		return false;
	}
}

bool SatUI::MyForm::cancelOutgoingTransfer(uint16_t tID) {
	if (backgroundWorker_Downlink->count(tID)) {
		if (backgroundWorker_Downlink[tID]->IsBusy) {
			backgroundWorker_Downlink[tID]->CancelAsync();
			System::Threading::Thread::CurrentThread->Sleep(100); //Stall for backgroundWorker_Downlink to stop
		}
		backgroundWorker_Downlink->erase(tID);
	}

	if (CommsNaSPUoN::outgoingTransfers.count(tID)) {
		CommsNaSPUoN::outgoingTransfers.erase(tID);
		if (transferForms->count(tID)) {
			transferForms[tID]->Close();
			transferForms->erase(tID);
		}
		log("CommHndl -> Incomplete outgoing transfer {" + std::to_string(tID) + "} has been deleted successfully.");
		return true;
	}
	else {
		log("The current outgoing File Transfer appears to have already completed.");
		return false;
	}
}

void SatUI::MyForm::cancelAllTransfers(std::vector<uint8_t> AX25GSCallsignSSID) {
	std::vector<uint8_t> payload;
	payload.push_back('T');
	payload.push_back('Z');
	payload.push_back(0xFF);
	try {
		for (cliext::map<uint16_t, System::ComponentModel::BackgroundWorker^>::iterator it = backgroundWorker_Downlink->begin(); it != backgroundWorker_Downlink->end();) {
			if (it->second->IsBusy) {
				it->second->CancelAsync();
				System::Threading::Thread::CurrentThread->Sleep(100); //Stall for backgroundWorker_Downlink to stop
				transferForms[it->first]->Close();
			}
			it = backgroundWorker_Downlink->erase(it);
		}
		if (this->backgroundWorker_UplinkPartRequest->IsBusy) {
			backgroundWorker_UplinkPartRequest->CancelAsync();
			System::Threading::Thread::CurrentThread->Sleep(100); //Stall for backgroundWorker_DownlinkPartRequest to stop
		}
		for (std::map<uint16_t, FileTransfer>::iterator it = CommsNaSPUoN::incomingTransfers.begin(); it != CommsNaSPUoN::incomingTransfers.end();) {
			it = CommsNaSPUoN::incomingTransfers.erase(it);
		}
		for (std::map<uint16_t, FileTransfer>::iterator it = CommsNaSPUoN::outgoingTransfers.begin(); it != CommsNaSPUoN::outgoingTransfers.end();) {
			it = CommsNaSPUoN::outgoingTransfers.erase(it);
		}
		payload.push_back(0x00);
		sendRFPacket(AX25GSCallsignSSID, payload);
		log("CommHndl -> All incomplete transfers have been deleted successfully.");
	}
	catch (...) {
		payload.push_back(0x01);
		sendRFPacket(AX25GSCallsignSSID, payload);
		log("CommHndl -> An error occurred when attempting to delete all incomplete transfers.");
	}

	for (cliext::map<uint16_t, DownlinkTransfer^>::iterator it = transferForms->begin(); it != transferForms->end();) {
		it = transferForms->erase(it);
	}
	adjuplinkComplete(false);
}


#endif
