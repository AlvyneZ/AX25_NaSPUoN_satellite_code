/*
   Author: AlvyneZ

   Code for NaSPUoN AX25 KISS TNC communication
*/


#ifndef AX25_COMMUNICATION_H
#define AX25_COMMUNICATION_H


#include <vector>
#include <string>
#include <map>
#include <chrono>


uint64_t timeSinceEpochMillisec() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

#define KISS_FEND  0xC0
#define KISS_FESC  0xDB
#define KISS_TFEND 0xDC
#define KISS_TFESC 0xDD

#define KISS_TYPE_DATA 0x00
#define KISS_TYPE_CMD  0x06

#define KISS_TNC_BUFFER_LIMIT 16384
#define KISS_OUT_BUFFER_LIMIT 100

namespace KISS {
	std::vector<uint8_t> kissInBuffer;
	std::vector< std::vector<uint8_t> > kissOutBuffer;
	uint64_t kissTNCBufferSize;
	bool awaitTNC = false;

	std::map<uint64_t, std::vector<uint8_t> > HPAX25AwaitingACK;
}



//Processing of received frames from the serial port
void SatUI::MyForm::processReceivedPacketAX25(std::vector<uint8_t> & packet) {
	//log("///=> " + vectorToHexString(packet));
	std::vector<std::vector<uint8_t>> receivedFrames = kissCombineFrame(packet);
	for (int i = 0; i < receivedFrames.size(); i++) {
		kissDecapsulate(receivedFrames[i]);
		//log(vectorToHexString(receivedFrames[i]));
	}
}


/*
 * Function to combine received bits since frames may come one bit at a time,
 * and check if full frames have been received
 * Then return the frames with Transposed FESC and FEND returned to normal
 * and delimiting FEND frames removed
*/
std::vector<std::vector<uint8_t>> SatUI::MyForm::kissCombineFrame(std::vector<uint8_t> & receivedMsg) {
	int start = KISS::kissInBuffer.size();
	int end = start + receivedMsg.size();
	//Add the whole of the received message to the buffer
	KISS::kissInBuffer.insert(KISS::kissInBuffer.end(), receivedMsg.begin(), receivedMsg.end());

	std::vector<std::vector<uint8_t>> receivedFrames;
	int last = 0;
	std::vector<uint8_t> temp;
	//Check through the original received message for a FEND Flag
	for (int i = start; i < end; i++) {
		if (KISS::kissInBuffer[i] == KISS_FEND) {
			//Clearing temp to save new frame
			temp.clear();
			//Copy the frame into receivedMsg ignoring the FEND flag at the end
			temp.insert(temp.begin(), KISS::kissInBuffer.begin() + last,
				KISS::kissInBuffer.begin() + i);
			//Setting the end of the last frame tht was taken
			last = i + 1;
			//Removing FEND flags from the frame and transposing TFEND and TFESC back
			for (int j = temp.size() - 1; j >= 0; j--) {
				if (temp[j] == KISS_FEND)
					temp.erase(temp.begin() + j);
				else if (temp[j] == KISS_FESC) {
					if (temp[j + 1] == KISS_TFEND)
						temp[j] = KISS_FEND;
					else if (temp[j + 1] == KISS_TFESC)
						temp[j] = KISS_FESC;
					else {
						logErr("AX25 -> KISS packet discarded due to transposing error: " + vectorToHexString(temp));
						temp.clear();
						break;
					}
					temp.erase(temp.begin() + j + 1);
				}
			}
			if (!temp.empty())
				receivedFrames.push_back(temp);
		}
	}
	KISS::kissInBuffer.erase(KISS::kissInBuffer.begin(), KISS::kissInBuffer.begin() + last);
	return receivedFrames;
}


/*
 * Function to check kiss packet data type and TNCPort.
*/
void SatUI::MyForm::kissDecapsulate(std::vector<uint8_t> & receivedFrame) {
	//If the reseived packet is from the wrong TNCPort, drop it
	if ((receivedFrame[0] & 0xF0) != this->TNCPort)
		return;
	uint8_t kissPacketType = receivedFrame[0] & 0x0F;
	receivedFrame.erase(receivedFrame.begin());

	std::vector<uint8_t> AX25SatCallsignSSID;
	int temp;
	switch (kissPacketType) {
	case KISS_TYPE_DATA:
		AX25SatCallsignSSID = ax25Decapsulate(receivedFrame);
		if (AX25SatCallsignSSID.size() > 0) {
			HPAX25PacketDecapsulate(AX25SatCallsignSSID, receivedFrame);
			processIncomingPayload(AX25SatCallsignSSID, receivedFrame);
		}
		break;
	case KISS_TYPE_CMD:
		//To refresh the KISS TNC Buffer Used Size
		if (receivedFrame.size() > 6) {
			if ((receivedFrame[0] == 'T') && (receivedFrame[1] == 'X') && (receivedFrame[2] == 'B') &&
			(receivedFrame[3] == 'U') && (receivedFrame[4] == 'F') && (receivedFrame[5] == ':')) {
				KISS::kissTNCBufferSize = 0;
				for (int i = 6; i < receivedFrame.size(); i++) {
					temp = receivedFrame[i] - '0';
					if ((temp < 10) && (temp >= 0)) {
						KISS::kissTNCBufferSize *= 10;
						KISS::kissTNCBufferSize += temp;
					}
				}
				if (KISS::kissTNCBufferSize >= KISS_TNC_BUFFER_LIMIT)
					KISS::awaitTNC = true;
				else
					KISS::awaitTNC = false;
			}
		}
		break;
	}
}


/*
 * Function to add data type and TNCPort to kiss frame
 * It also adds FEND flags and transposes FENDs and FESCs in the frame
*/
void SatUI::MyForm::kissEncapsulate(bool command, std::vector<uint8_t> & outgoingMsg) {
	if (command) {
		outgoingMsg.insert(outgoingMsg.begin(), (this->TNCPort | KISS_TYPE_CMD));
	}
	else {
		outgoingMsg.insert(outgoingMsg.begin(), (this->TNCPort | KISS_TYPE_DATA));
	}
	kissTranspose(outgoingMsg);
	outgoingMsg.insert(outgoingMsg.begin(), KISS_FEND);
	outgoingMsg.insert(outgoingMsg.end(), KISS_FEND);
}


/*
 * Function to transpose FEND and FESC to FESC-TFEND and FESC-TFESC respectively
*/
void SatUI::MyForm::kissTranspose(std::vector<uint8_t> & outgoingMsg) {
	for (int i = outgoingMsg.size() - 1; i >= 0; i--) {
		if (outgoingMsg[i] == KISS_FEND) {
			outgoingMsg[i] = KISS_TFEND;
			outgoingMsg.insert(outgoingMsg.begin() + i, KISS_FESC);
		}
		else if (outgoingMsg[i] == KISS_FESC) {
			outgoingMsg[i] = KISS_TFESC;
			outgoingMsg.insert(outgoingMsg.begin() + i, KISS_FESC);
		}
	}
}




/*
	This AX.25 UI implementation has been written considering the following:
		Addition of AX.25 flags (0x7E) is done by the TNC
		Bit Stuffing to avoid the occurence of more than 5 consecutive 1's
			is also done by the TNC
		There are no repeaters to be passed through by the frames
		The Frame Check Sequence calculation is done by the TNC
*/


#define AX25_UI_CONTROL		0x03
#define AX25_NO_PROT_PID	0xF0
#define AX25_SSID_PREFIX	0x30

#define APRS_MORSE_DATA_IND	'm'

#define AX25_RESEND_LIMIT 10
#define AX25_RESEND_INTERVAL_MILLI	1000

std::vector<uint8_t> SatUI::MyForm::ax25Decapsulate(std::vector<uint8_t> & kissdecappedMsg) {
	std::vector<uint8_t> ret;
	//Getting the index of the first byte of the source in the address field
	int sourceAddress = 0;
	do {
		sourceAddress += 7;
		if ((sourceAddress + 6) >= kissdecappedMsg.size()) {
			logErr("AX25 -> Error (source address) when performing AX.25 decapsulation on message: " + vectorToHexString(kissdecappedMsg));
			return ret;
		}
	} while (!(kissdecappedMsg[sourceAddress + 6] & 0x01));
	//Shifting the bytes in the address field right
	for (int i = 0; i < (sourceAddress + 7); i++) {
		kissdecappedMsg[i] >>= 1;
	}
	//Getting the destination and source addresses
	std::vector<uint8_t> msgDestination(kissdecappedMsg.begin(), kissdecappedMsg.begin() + 6);
	uint8_t msgDestinationSSID = kissdecappedMsg[6] & 0x0F;
	std::vector<uint8_t> msgSource(kissdecappedMsg.begin() + sourceAddress, kissdecappedMsg.begin() + sourceAddress + 6);
	uint8_t msgSourceSSID = kissdecappedMsg[sourceAddress + 6] & 0x0F;
	//Removing spaces from addresses
	for (int i = (msgDestination.size() - 1); i >= 0; i--) {
		if (msgDestination[i] == ' ') msgDestination.erase(msgDestination.begin() + i);
	}
	for (int i = (msgSource.size() - 1); i >= 0; i--) {
		if (msgSource[i] == ' ') msgSource.erase(msgSource.begin() + i);
	}

	//If message is not meant for the ground station address, drop it
	std::vector<uint8_t> groundCallsign = this->getGroundCallsignSSID();
	if ((msgDestination.size() + 1) != groundCallsign.size())
		return ret;
	for (int i = 0; i < msgDestination.size(); i++) {
		if (msgDestination[i] != groundCallsign[i])
			return ret;
	}
	if (msgDestinationSSID != groundCallsign[groundCallsign.size() - 1])
		return ret;

	//Getting the actual message by removing address field, control field and protocol field
	kissdecappedMsg.erase(kissdecappedMsg.begin(), kissdecappedMsg.begin() + sourceAddress + 9);

	//If data frame is not an APRS morse data type, drop the frame
	if (kissdecappedMsg[0] != APRS_MORSE_DATA_IND)
		return ret;

	//Remove the APRS morse data type indicator
	kissdecappedMsg.erase(kissdecappedMsg.begin());

	//Returning the source(satellite address)
	ret.insert(ret.begin(), msgSource.begin(), msgSource.end());
	ret.push_back(msgSourceSSID);
	return ret;
}

void SatUI::MyForm::ax25Encapsulate(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & outgoingMsg) {
	outgoingMsg.insert(outgoingMsg.begin(), APRS_MORSE_DATA_IND);
	outgoingMsg.insert(outgoingMsg.begin(), AX25_NO_PROT_PID);
	outgoingMsg.insert(outgoingMsg.begin(), AX25_UI_CONTROL);

	std::vector<uint8_t> source = this->getGroundCallsignSSID();
	source.pop_back();
	while (source.size() < 6) source.push_back(' ');
	source.push_back(this->groundSSID | AX25_SSID_PREFIX);
	for (int i = 0; i < source.size(); i++) {
		source[i] <<= 1;
	}
	source[6] |= 0x01;
	outgoingMsg.insert(outgoingMsg.begin(), source.begin(), source.end());

	std::vector<uint8_t> dest(AX25SatCallsignSSID.begin(), AX25SatCallsignSSID.end()-1);
	for (int i = dest.size(); i < 6; i++) dest.push_back(' ');
	dest.push_back(AX25SatCallsignSSID[AX25SatCallsignSSID.size()-1] | AX25_SSID_PREFIX);
	for (int i = 0; i < dest.size(); i++) {
		dest[i] <<= 1;
	}
	outgoingMsg.insert(outgoingMsg.begin(), dest.begin(), dest.end());
}


void SatUI::MyForm::sendRFPacketAX25(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & packet) {
	HPAX25PacketEncapsulate(AX25SatCallsignSSID, packet);
	ax25Encapsulate(AX25SatCallsignSSID, packet);
	kissEncapsulate(false, packet);
	
	//Requesting the TNC to send its buffer used up space
	checkKissTNCBufferSize();

	//Adding the packet to send to the output buffer
	msclr::lock lck(kissOutMutex);
	KISS::kissOutBuffer.push_back(packet);
	lck.release();
}


void SatUI::MyForm::checkKissTNCBufferSize() {
	std::string cmd = "TXBUF:";
	std::vector<uint8_t> outgoingCmd(cmd.begin(), cmd.end());
	kissEncapsulate(true, outgoingCmd);
	sendSerial(outgoingCmd);
}


void SatUI::MyForm::sendAX25Frames() {
	System::Threading::Thread::CurrentThread->Sleep(100);

	//Adding High Priority Packets that are meant to be resent to the Out Buffer
	msclr::lock lck0(HPAX25Mutex);
	for (std::map<uint64_t, std::vector<uint8_t>>::iterator it = KISS::HPAX25AwaitingACK.begin(); it != KISS::HPAX25AwaitingACK.end(); it++) {
		std::vector<uint8_t> AX25SatCallsignSSID((it->second).begin(), (it->second).begin() + 8);
		std::vector<uint8_t> packet((it->second).begin() + 8, (it->second).end());
		uint8_t resendCount = packet[3];
		if (resendCount >= AX25_RESEND_LIMIT) {
			KISS::HPAX25AwaitingACK.erase(it);
		}
		else {
			uint64_t currentTimeStamp = timeSinceEpochMillisec();
			uint64_t nextResendTimeStamp = ((1+resendCount) * AX25_RESEND_INTERVAL_MILLI);
			nextResendTimeStamp += getSixtyFourBitIntFromEightBitVector(packet, 4);
			if (currentTimeStamp > nextResendTimeStamp) {
				packet[3] ++;
				sendRFPacketAX25(AX25SatCallsignSSID, packet);
			}
		}
	}
	lck0.release();

	//sending AX.25 packets that are in the Out Buffer
	checkKissTNCBufferSize();
	msclr::lock lck(kissOutMutex);
	int index = 0;
	for (; index < KISS::kissOutBuffer.size(); index++) {
		if (KISS::awaitTNC) {
			break;
		}
		sendSerial(KISS::kissOutBuffer[index]);
		checkKissTNCBufferSize();
	}
	if (index > 0)
		KISS::kissOutBuffer.erase(KISS::kissOutBuffer.begin(), KISS::kissOutBuffer.begin() + index);
	lck.release();
}



/*
 * To prevent errors in transmission, delivery of some packets need to be ensured
 *   These packets are: - Transmission Initialization (TI)
 *						- Transmission Begin (TB)
 *						- Transmission Complete (TC)
 *	  To ensure delivery Acknowledgements will be used
*/

//Function to check if a packet is High Priority(HP)
bool SatUI::MyForm::isOutAX25PacketHP(std::vector<uint8_t> & frameToSend) {
	try {
		if (frameToSend[0] == 'T') {
			return ((frameToSend[1] == 'I') || (frameToSend[1] == 'B') || (frameToSend[1] == 'C'));
		}
		return false;
	}
	catch (...) {
		return false;
	}
}

//Function that encapsulates frames that are high priority with an ID and 'HP' tag
void SatUI::MyForm::HPAX25PacketEncapsulate(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & frameToSend) {
	if (isOutAX25PacketHP(frameToSend)) {
		uint64_t HPID = timeSinceEpochMillisec();
		insertSixtyFourBitIntInEightBitVector(frameToSend, frameToSend.begin(), HPID);
		frameToSend.insert(frameToSend.begin(), 0x00);
		frameToSend.insert(frameToSend.begin(), 'D');
		frameToSend.insert(frameToSend.begin(), 'P');
		frameToSend.insert(frameToSend.begin(), 'H');

		std::vector<uint8_t> frameToSave(frameToSend.begin(), frameToSend.end());
		for (int i = AX25SatCallsignSSID.size(); i < 8; i++)
			AX25SatCallsignSSID.push_back(';');
		frameToSave.insert(frameToSave.begin(), AX25SatCallsignSSID.begin(), AX25SatCallsignSSID.end());
		
		KISS::HPAX25AwaitingACK.insert(std::make_pair(HPID, frameToSave));
	}

	//If the packet is not a high priority packet, do nothing
}

//Function that removes the 'HP' tag from received frames that are High Priority
//	and sends back the corresponding ACK
//It also checks if the received packet is an ACK
void SatUI::MyForm::HPAX25PacketDecapsulate(std::vector<uint8_t> AX25SatCallsignSSID, std::vector<uint8_t> & receivedFrame) {
	if ((receivedFrame.size() >= 11) && (receivedFrame[0] == 'H') && (receivedFrame[1] == 'P')) {
		if (receivedFrame[2] == 'A') {
			//Acknowledgement packet -> Remove from High Pririty packets to be resent

			//Extracting the High Priority packet ID
			uint64_t HPID = getSixtyFourBitIntFromEightBitVector(receivedFrame, 3);

			msclr::lock lck(this->HPAX25Mutex);
			if (KISS::HPAX25AwaitingACK.count(HPID)) {
				KISS::HPAX25AwaitingACK.erase(HPID);
			}
			lck.release();
			receivedFrame.clear();
		}
		else if (receivedFrame[2] == 'D') {
			//Data Packet -> Send back an Acknowledgement packet
			std::vector<uint8_t> ack(receivedFrame.begin(), receivedFrame.begin() + 12);
			ack[2] = 'A';
			ack.erase(ack.begin() + 3);
			sendRFPacketAX25(AX25SatCallsignSSID, ack);
			receivedFrame.erase(receivedFrame.begin(), receivedFrame.begin() + 12);
		}
		else
			return;
	}

	//If the packet is not a high priority packet, do nothing
}

#endif
