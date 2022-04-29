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
					array<uint16_t>^ args = gcnew array<uint16_t>(2);
					args[0] = (ft->packetsReceived * 100) / ft->expectedPackets;
					args[1] = tID;
					downlinkProgressBarUpdate(args);
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

bool SatUI::MyForm::transferComplete(uint16_t tID, std::vector<uint8_t> AX25SatCallsignSSID) {
	///log("Transfer Complete Called with: rcv=" + std::to_string(ft->packetsReceived) + " and expctd=" + std::to_string(ft->expectedPackets));
	if ((CommsNaSPUoN::incomingTransfers[tID].packetsReceived) >= (CommsNaSPUoN::incomingTransfers[tID].expectedPackets)) {
		std::string outputPath = getDownlinkSavelocation();
		outputPath += '\\';
		CommsNaSPUoN::incomingTransfers[tID].fileName[2] = '\\'; //Since the sat(Linux) uses forward-slashes but windows uses back-slashes
		outputPath += CommsNaSPUoN::incomingTransfers[tID].fileName;
		compilePackets(outputPath, CommsNaSPUoN::incomingTransfers[tID].packets);
		array<uint16_t>^ args = gcnew array<uint16_t>(2);
		args[0] = 100;
		args[1] = tID;
		downlinkProgressBarUpdate(args);
		return true;
	}
	else {
		if (backgroundWorker_DownlinkPartRequest->IsBusy) {
			backgroundWorker_DownlinkPartRequest->CancelAsync();
			log("CommHndl -> Cancelled downlink part request thread to allow for a new downlink part request transfer.");
			//Stalling to allow the thread to stop
			System::Threading::Thread::CurrentThread->Sleep(100);
		}
		array<uint8_t>^ args = gcnew array<uint8_t>(9);
		args[1] = tID & 0x00ff;
		args[0] = tID >> 8;
		int index = 0;
		for (; index < AX25SatCallsignSSID.size(); index++) {
			args[2 + index] = AX25SatCallsignSSID[index];
		}
		for (; index < 7; index++) {
			args[2 + index] = ';';
		}

		startBackgroundWorkerDownlinkPartRequest(args);

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

			ft.fileName = "";
			for (unsigned int i = 3; i < initializerPacket.size(); i++)
				ft.fileName += initializerPacket[i];

			if (checkIfFileExists(ft.fileName)) {
				std::vector<uint8_t> file = retrieveFileToTransfer(ft.fileName);
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


void SatUI::MyForm::sendRFPacket(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & packet) {
	//This function sends an RF packet with either or both the XBee and/or StenSat
	if (packet.size() > 1) {
		sendRFPacketAX25(AX25SatCallsignSSID, packet);
	}
	else {
		logErr("CommHndl -> Attempt to send an Rf packet of size: " + std::to_string(packet.size()) + " denied.");
	}
}

void SatUI::MyForm::uplinkTransfer(std::vector<uint8_t> AX25SatCallsignSSID, uint16_t tID, System::ComponentModel::DoWorkEventArgs^  e) {
	log("CommHndl -> Beginning outgoing transfer of tID " + std::to_string(tID) + ".");
	std::vector<uint8_t> pck;
	uint32_t expectedPackets = CommsNaSPUoN::outgoingTransfers[tID].expectedPackets;
	uint8_t lastFrameID;
	for (uint32_t i = 0; i < expectedPackets; i++) {
		pck = getPacket(&(CommsNaSPUoN::outgoingTransfers[tID]), i);
		//msclr::lock lck(this->uplinkSendNextMutex);
		sendRFPacket(AX25SatCallsignSSID, pck);

		while (KISS::awaitTNC || (KISS::kissOutBuffer.size() > KISS_OUT_BUFFER_LIMIT)) {
			if (this->backgroundWorker_Uplink->CancellationPending) {
				e->Cancel = true;
				return;
			}
		}

		uplinkProgressBarUpdate((i * 100) / expectedPackets);
	}

	pck.clear();
	pck.push_back('T');
	pck.push_back('C');
	insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);
	pck.push_back(0x01);
	sendRFPacket(AX25SatCallsignSSID, pck);
	uplinkProgressBarUpdate(100);
}

void SatUI::MyForm::downlinkPartRequestTransfer(std::vector<uint8_t> AX25SatCallsignSSID, uint16_t tID, System::ComponentModel::DoWorkEventArgs^  e) {

	uint32_t expectedPackets = CommsNaSPUoN::incomingTransfers[tID].expectedPackets;
	std::vector<uint8_t> pck;
	uint8_t lastFrameID;
	for (uint32_t i = 0; i < expectedPackets; i++) {
		if (this->backgroundWorker_DownlinkPartRequest->CancellationPending) {
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
			//msclr::lock lck(this->uplinkSendNextMutex);
			sendRFPacket(AX25SatCallsignSSID, pck);

			while (KISS::awaitTNC || (KISS::kissOutBuffer.size() > KISS_OUT_BUFFER_LIMIT)) {
				if (this->backgroundWorker_DownlinkPartRequest->CancellationPending) {
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
	sendRFPacket(AX25SatCallsignSSID, pck);
}

void SatUI::MyForm::processIncomingPayload(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & payload) {
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
						sendRFPacket(AX25SatCallsignSSID, getInitializationPacket(&newFileTransfer));
						log("CommHndl -> Transfer request received and accepted.");
					}
				}
				else if (payload[2] == 0x03) {
					//Count of packets that were successfully received
					uint16_t tID = getSixteenBitIntFromEightBitVector(payload, 3);

					if (CommsNaSPUoN::outgoingTransfers.count(tID)) {
						uint32_t received = getThirtyTwoBitIntFromEightBitVector(payload, 5);
						uplinkProgressBarUpdate((received * 100) / CommsNaSPUoN::outgoingTransfers[tID].expectedPackets);
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
						sendRFPacket(AX25SatCallsignSSID, getPacket(&(CommsNaSPUoN::outgoingTransfers[tID]), pckIndex));
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
					log("CommHndl -> Intitializer packet for downlink transfer tID: " + std::to_string(tID) + " accepted." + expPcks);
					downlinkStarting(tID);
				}
				else {
					pck.push_back(0x01);
					log("CommHndl -> Intitializer packet for downlink transfer tID: " + std::to_string(tID) + " rejected.");
				}

				sendRFPacket(AX25SatCallsignSSID, pck);
			}
			else if (payload[1] == 'B') {
				//Request to begin transfering packets of a file
				if (payload.size() < 5) //To avoid accessing outside vector size
					return;
				uint16_t tID = getSixteenBitIntFromEightBitVector(payload, 2);
				if (payload[4] == 0x00) {
					if (backgroundWorker_Uplink->IsBusy) {
						backgroundWorker_Uplink->CancelAsync();
						log("CommHndl -> Cancelled uplink thread to allow for a new uplink transfer.");
						//Stalling to allow the thread to stop
						System::Threading::Thread::CurrentThread->Sleep(100);
					}
					array<uint8_t>^ args = gcnew array<uint8_t>(9);
					args[1] = tID & 0x00ff;
					args[0] = tID >> 8;
					int index = 0;
					for (; index < AX25SatCallsignSSID.size(); index++) {
						args[2 + index] = AX25SatCallsignSSID[index];
					}
					for (; index < 7; index++) {
						args[2 + index] = ';';
					}

					startBackgroundWorkerUplink(args);
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
					bool complete = transferComplete(tID, AX25SatCallsignSSID);
					if (complete) {
						CommsNaSPUoN::incomingTransfers.erase(tID);
						downlinkComplete(tID);
						std::vector<uint8_t> pck;
						pck.push_back('T');
						pck.push_back('C');
						insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);
						pck.push_back(0x00);
						sendRFPacket(AX25SatCallsignSSID, pck);
						log("CommHndl -> Successfully Completed incoming transfer of tID " + std::to_string(tID) + ".");
					}
				}
				else if ((payload[4] == 0x00) && (CommsNaSPUoN::outgoingTransfers.count(tID))) {
					CommsNaSPUoN::outgoingTransfers.erase(tID);
					log("CommHndl -> Successfully Completed outgoing transfer of tID " + std::to_string(tID) + ".");
				}
				else if ((payload[4] == 0x02) && (CommsNaSPUoN::outgoingTransfers.count(tID))) {
					std::vector<uint8_t> pck;
					pck.push_back('T');
					pck.push_back('C');
					insertSixteenBitIntInEightBitVector(pck, pck.end(), tID);
					pck.push_back(0x01);
					sendRFPacket(AX25SatCallsignSSID, pck);
				}
				else {
					std::string outOrIn = (payload[4] == 0x01) ? "Incoming" : ((payload[4] == 0x00) || (payload[4] == 0x02)) ? "Outgoing" : "[Direction Unknown]";
					logErr("CommHndl -> Received a transfer complete for a non-existent " + outOrIn + " tID " + std::to_string(tID) + ".");
				}
			}
			else if (payload[1] == 'Z') {
				//Cancel Transfer response
				if (payload.size() < 4) //To avoid accessing outside vector size
					return;
				if (payload[2] == 0xFF) {
					//Cancel all incomplete transfers response
					if (payload[3] == 0x00)
						log("CommHndl -> Satelitte has successfully deleted all incomplete transfers.");
					else if (payload[3] == 0x01)
						log("CommHndl -> Satelitte encountered an error when attempting to delete all incomplete transfers.");
				}
				if (payload[2] == 0x00) {
					//Cancel a specific incomplete transfer response
					if (payload.size() < 7) //To avoid accessing outside vector size
						return;
					uint16_t tID = getSixteenBitIntFromEightBitVector(payload, 4);
					std::string direction = (payload[3] == 0x01) ? "uplink" : (payload[3] == 0x00) ? "downlink" : "{unknown direction}";
					if (payload[6] == 0x00)
						log("CommHndl -> Satelitte has successfully deleted " + direction + " incomplete transfer {" + std::to_string(tID) + "}.");
					else if (payload[6] == 0x01)
						log("CommHndl -> Satelitte didn't delete " + direction + " non-existent transfer {" + std::to_string(tID) + "}.");
					else if (payload[6] == 0x0F)
						log("CommHndl -> Satelitte encountered an error deleting " + direction + " incomplete transfer {" + std::to_string(tID) + "}.");
				}
			}
		}
		else if (payload[0] == 'L') {
			if (payload[1] == 'M') {
				//Response for saving current log file to old log files folder
				if (payload.size() < 3) //To avoid accessing outside vector size
					return;
				if (payload[2] == 0)
					log("CommHndl -> Current Log file has been successfully saved to Old Logs Folder.");
				else
					log("CommHndl -> An error (" + std::to_string(payload[2]) + ") occurred when attempting to save log file to Old Logs Folder.");
			}
			else if (payload[1] == 'F') {
				//Response of file names of lora files
				if (payload.size() < 3) //To avoid accessing outside vector size
					return;
				if (payload[2] == 0x00) {
					log("CommHndl -> There are " + std::to_string(payload[3]) + " old log files on the satellite.");
				}
				else if (payload[2] == 0x01) {
					log("CommHndl -> There was an error finding how many old log files are on the satellite.");
				}
				else if (payload[2] == 0x0F) {
					payload[0] = 'l';
					payload[1] = 'f';
					payload[2] = '/';
					std::string file(payload.begin(), payload.end());
					System::String^ fileSys = gcnew System::String(file.c_str());
					addDownlinkableFileName(fileSys);
				}
			}
			else if (payload[1] == 'R') {
				//Response of file names of old log files
				if (payload.size() < 3) //To avoid accessing outside vector size
					return;
				if (payload[2] == 0x00) {
					log("CommHndl -> There are " + std::to_string(payload[3]) + " LoRa packets on the satellite.");
				}
				else if (payload[2] == 0x01) {
					log("CommHndl -> There was an error finding how many LoRa packets are on the satellite.");
				}
				else if (payload[2] == 0x0F) {
					payload[0] = 'l';
					payload[1] = 'r';
					payload[2] = '/';
					std::string file(payload.begin(), payload.end());
					System::String^ fileSys = gcnew System::String(file.c_str());
					addDownlinkableFileName(fileSys);
				}
			}
		}
		else if ((payload[0] == 'I') && (payload[1] == 'M')) {
			//Response of the file names of images present on the satellite
			if (payload.size() < 3) //To avoid accessing outside vector size
				return;
			if (payload[2] == 0x00) {
				log("CommHndl -> There are " + std::to_string(payload[3]) + " images on the satellite.");
			}
			else if (payload[2] == 0x01) {
				log("CommHndl -> There was an error finding how many images are on the satellite.");
			}
			else if (payload[2] == 0x0F) {
				payload[0] = 'i';
				payload[1] = 'm';
				payload[2] = '/';
				std::string file(payload.begin(), payload.end());
				System::String^ fileSys = gcnew System::String(file.c_str());
				addDownlinkableFileName(fileSys);
			}
		}
		else if ((payload[0] == 'X') && (payload[1] == 'C')) {
			//Response for saving uploaded XBee Config file
			if (payload.size() < 3) //To avoid accessing outside vector size
				return;
			if (payload[2] == 0x00) {
				log("CommHndl -> Satellite Successfully copied the config file.");
			}
			else if (payload[2] == 0x10) {
				log("CommHndl -> Satellite Successfully copied the config file and successfully setup the xbee with the new settings.");
			}
			else if (payload[2] == 0x01) {
				logErr("CommHndl -> Satellite failed to copy config file from uplink location. File not found.");
			}
			else if (payload[2] == 0x02) {
				logErr("CommHndl -> Satellite failed to copy config file from uplink location due to error.");
			}
			else if (payload[2] == 0x04) {
				logErr("CommHndl -> Satellite Successfully copied the config file BUT failed to setup the xbee with the new settings.");
			}
		}
		else if ((payload[0] == 'D') && (payload[1] == 'R')) {
			//Payload for sending date and time to the satellite
			if (payload.size() != 2) //To avoid errors
				return;
			payload[1] = 'S';

			//Retrieving system time
			time_t now = time(0);
			time_t *timeNow = &now;
			tm *localTime = localtime(timeNow);

			//Year
			uint16_t year = localTime->tm_year + 1900;
			payload.push_back((uint8_t)(year / 256)); //(year >> 8)
			payload.push_back((uint8_t)(year % 256)); //(year & 0x00ff)
			//Month
			payload.push_back((uint8_t)(localTime->tm_mon + 1));
			//Day of the month
			payload.push_back((uint8_t)(localTime->tm_mday));

			//Time -> Hour, Minutes and Seconds
			payload.push_back((uint8_t)(localTime->tm_hour));
			payload.push_back((uint8_t)(localTime->tm_min));
			payload.push_back((uint8_t)(localTime->tm_sec));

			sendRFPacket(AX25SatCallsignSSID, payload);

			log("CommHndl -> Sent time to the satellite.");
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

void SatUI::MyForm::cancelIncomingTransfer(uint16_t tID) {
	if (this->backgroundWorker_DownlinkPartRequest->IsBusy) {
		backgroundWorker_DownlinkPartRequest->CancelAsync();
		System::Threading::Thread::CurrentThread->Sleep(50); //Stall for backgroundWorker_DownlinkPartRequest to stop
	}
	if (CommsNaSPUoN::incomingTransfers.count(tID)) {
		CommsNaSPUoN::incomingTransfers.erase(tID);
		if (transferForms->count(tID)) {
			transferForms[tID]->Close();
			transferForms->erase(tID);
		}
	}
	else {
		log("The current incoming File Transfer appears to have already completed.");
	}
}

void SatUI::MyForm::newOutgoingTransfer(std::vector<uint8_t> AX25SatCallsignSSID, std::string fileNameWithPath) {
	FileTransfer ft;

	std::string fileName = "";
	for (int i = (fileNameWithPath.size() - 1); ((i > 0) && (fileNameWithPath[i] != '\\')); i--) {
		fileName.insert(fileName.begin(), fileNameWithPath[i]);
	}

	if (checkIfFileExists(fileNameWithPath)) {
		std::vector<uint8_t> file = retrieveFileToTransfer(fileNameWithPath);
		ft.fileSize = file.size();
		ft.fileName = fileName;

		ft.packets = transferOutPacketsConstruction(ft.transferID, ft.fileName, file);
		ft.expectedPackets = ft.packets.size() - 1;

		CommsNaSPUoN::outgoingTransfers[ft.transferID] = ft;
		this->currentUplinkTransferID = ft.transferID;
		sendRFPacket(AX25SatCallsignSSID, getInitializationPacket(&ft));
		log("CommHndl -> Transfer initialization packet sent.");
	}
	else {
		logErr("CommHndl -> Request to transfer out '" + fileNameWithPath + "' denied since file cannot be found.");
	}
}

void SatUI::MyForm::cancelOutgoingTransfer(uint16_t tID) {
	if (this->backgroundWorker_Uplink->IsBusy) {
		backgroundWorker_Uplink->CancelAsync();
		System::Threading::Thread::CurrentThread->Sleep(50); //Stall for backgroundWorker_Uplink to stop
	}
	if (CommsNaSPUoN::outgoingTransfers.count(tID)) {
		CommsNaSPUoN::outgoingTransfers.erase(tID);
	}
	else {
		log("The current outgoing File Transfer appears to have already completed.");
	}
}

void SatUI::MyForm::cancelAllTransfers() {
	if (this->backgroundWorker_Uplink->IsBusy) {
		backgroundWorker_Uplink->CancelAsync();
		System::Threading::Thread::CurrentThread->Sleep(50); //Stall for backgroundWorker_Uplink to stop
	}
	if (this->backgroundWorker_DownlinkPartRequest->IsBusy) {
		backgroundWorker_DownlinkPartRequest->CancelAsync();
		System::Threading::Thread::CurrentThread->Sleep(50); //Stall for backgroundWorker_DownlinkPartRequest to stop
	}

	std::vector<uint8_t> payload;
	payload.push_back('T');
	payload.push_back('Z');
	payload.push_back(0xFF); //Delete all incomplete transfers

	sendRFPacket(this->getSatelliteCallsignSSID(), payload);

	for (std::map<uint16_t, FileTransfer>::iterator it = CommsNaSPUoN::incomingTransfers.begin(); it != CommsNaSPUoN::incomingTransfers.end(); it++) {
		CommsNaSPUoN::incomingTransfers.erase(it);
	}
	for (std::map<uint16_t, FileTransfer>::iterator it = CommsNaSPUoN::outgoingTransfers.begin(); it != CommsNaSPUoN::outgoingTransfers.end(); it++) {
		CommsNaSPUoN::outgoingTransfers.erase(it);
	}
	log("CommHndl -> All incomplete transfers have been deleted successfully.");

	for (cliext::map<uint16_t, IncomingTransfer^>::iterator it = transferForms->begin(); it != transferForms->end(); it++) {
		transferForms->erase(it);
	}
	uplinkComplete(false);
}


#endif
