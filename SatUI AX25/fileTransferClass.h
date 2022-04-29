


#ifndef FILE_TRANSFER_CLASS_H
#define FILE_TRANSFER_CLASS_H


#include <vector>
#include <string>
#include <map>


class FileTransfer {
public:
	bool ready;

	uint16_t transferID;
	uint32_t packetsReceived;
	uint32_t fileSize;
	uint32_t expectedPackets;
	std::string fileName;
	std::vector< std::vector<uint8_t> > packets;
	std::vector<bool> receivedPacket;
};


namespace CommsNaSPUoN
{
	//Variables to hold all file transfers {Note: they should be deleted upon completion of the transfer}
	std::map<uint16_t, FileTransfer > outgoingTransfers;
	std::map<uint16_t, FileTransfer > incomingTransfers;

	//For the threads (Uplink & downlink part request) to know when to send the next rf packet
	//std::map<uint8_t, bool> uplinkSendNext;
}

#endif