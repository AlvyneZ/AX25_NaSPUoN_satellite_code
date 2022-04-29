/*
   Author: AlvyneZ

   Code for NaSPUoN File Operations

   Note: This file should NOT be included before Logger.h
*/

#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H



#include <vector>
#include <fstream>
#include <string>

#define MAX_PACKET_SIZE		100 //Bytes
#define PACKET_HEADER_SIZE	24 //Bytes




bool SatUI::MyForm::checkIfFileExists(std::string file_path) {
	//This Function checks if a file (whose path is known) exists
	//The function returns true if it exists.

	std::ifstream input;
	input.open(file_path.c_str(), std::ios::binary | std::ios::in);
	bool exists = input.is_open();
	input.close();
	return exists;
}

uint32_t SatUI::MyForm::getFileSize(std::string file_path) {
	//This Function gets the size of a file in bytes
	//The function returns the size as a 32 bit unsigned integer

	uint32_t fileSize;
	std::ifstream file;
	file.open(file_path.c_str(), std::ios::binary | std::ios::in);

	//Return zero if the file doesn't exist
	if (!(file.is_open()))
		return 0;

	file.seekg(0, std::ios::end);
	std::streampos fileSizeSP = file.tellg();
	file.seekg(0, std::ios::beg);

	file.close();

	fileSize = (uint32_t)fileSizeSP;
	return fileSize;
}


std::vector<uint8_t> SatUI::MyForm::retrieveFileToTransfer(std::string input_path) {
	//This function copies the contents of a file to a vector
	//The function returns a vector whose size equals the fileSize

	std::ifstream input_file;
	input_file.open(input_path.c_str(), std::ios::binary | std::ios::in);

	//Return an empty vector if the file doesn't exist
	if (!(input_file.is_open())) {
		logErr("The file specified for input '" + input_path + "' does not exist.");
		std::vector<uint8_t> fileData;
		return fileData;
	}

	uint32_t fileSize = getFileSize(input_path);

	std::vector<uint8_t> fileData(fileSize);
	input_file.read((char*)&fileData[0], fileSize);
	input_file.close();

	return fileData;
}

std::vector< std::vector<uint8_t> > SatUI::MyForm::splitFileIntoPackets(std::vector<uint8_t> file) {
	//This function splits the vector containing the file contents into several packets that can be sent
	//The size of the packets is calculated from the packet header that would be added and the max size

	uint32_t packetDataSize = MAX_PACKET_SIZE - PACKET_HEADER_SIZE;
	std::vector<uint8_t> pck;
	std::vector< std::vector<uint8_t> > packets;
	uint32_t i;
	for (i = 0; (i + packetDataSize) < file.size(); i += packetDataSize) {
		pck.clear();
		pck.insert(pck.begin(), file.begin() + i, file.begin() + i + packetDataSize);
		packets.push_back(pck);
	}
	pck.clear();
	pck.insert(pck.begin(), file.begin() + i, file.end());
	if (pck.size() > 0)
		packets.push_back(pck);

	return packets;
}

void SatUI::MyForm::compilePackets(std::string output_path, std::vector< std::vector<uint8_t> > packets) {
	//This function takes the packets received and combines them into a single 1D vector
	//The function then outputs the vector to a file

	std::vector<uint8_t> file;
	for (uint32_t i = 0; i < packets.size(); i++) {
		file.insert(file.end(), packets[i].begin(), packets[i].end());
	}

	if (checkIfFileExists(output_path)) {
		logErr("The file specified for output '" + output_path + "' already exists.");
		return;
	}
	log("Beginning output to '" + output_path + "'.");
	std::ofstream output_file;
	output_file.open(output_path.c_str(), std::ios::binary | std::ios::out | std::ios::app);
	for (unsigned int i = 0; i < file.size(); i++) {
		output_file << file[i];
	}
	log("Output to '" + output_path + "' Complete.");
	output_file.close();
}

#endif
