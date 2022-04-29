/*
	Author: AlvyneZ

	Program: Manipulations for changing uint16_t, uint32_t to vector of uint8_t
		and for changing vector of uint8_t to uint16_t, uint32_t
*/

#ifndef BIT_MANIPULATION_H
#define BIT_MANIPULATION_H

#include <vector>
#include <string>
#include <sstream>


std::vector<uint8_t> SatUI::MyForm::splitSixteenBitInt(uint16_t sixteenBitInt) {
	std::vector<uint8_t> eightBitIntsVector;
	eightBitIntsVector.push_back((uint8_t)((sixteenBitInt & 0xFF00) >> 8));
	eightBitIntsVector.push_back((uint8_t)(sixteenBitInt & 0x00FF));
	return eightBitIntsVector;
}

std::vector<uint8_t> SatUI::MyForm::splitThirtyTwoBitInt(uint32_t thirtyTwoBitInt) {
	std::vector<uint8_t> eightBitIntsVector;
	eightBitIntsVector.push_back((uint8_t)((thirtyTwoBitInt & 0xFF000000) >> 24));
	eightBitIntsVector.push_back((uint8_t)((thirtyTwoBitInt & 0x00FF0000) >> 16));
	eightBitIntsVector.push_back((uint8_t)((thirtyTwoBitInt & 0x0000FF00) >> 8));
	eightBitIntsVector.push_back((uint8_t)(thirtyTwoBitInt & 0x000000FF));
	return eightBitIntsVector;
}

std::vector<uint8_t> SatUI::MyForm::splitSixtyFourBitInt(uint64_t sixtyFourBitInt) {
	std::vector<uint8_t> eightBitIntsVector;
	eightBitIntsVector.push_back((uint8_t)((sixtyFourBitInt & 0xFF00000000000000) >> 56));
	eightBitIntsVector.push_back((uint8_t)((sixtyFourBitInt & 0x00FF000000000000) >> 48));
	eightBitIntsVector.push_back((uint8_t)((sixtyFourBitInt & 0x0000FF0000000000) >> 40));
	eightBitIntsVector.push_back((uint8_t)((sixtyFourBitInt & 0x000000FF00000000) >> 32));
	eightBitIntsVector.push_back((uint8_t)((sixtyFourBitInt & 0x00000000FF000000) >> 24));
	eightBitIntsVector.push_back((uint8_t)((sixtyFourBitInt & 0x0000000000FF0000) >> 16));
	eightBitIntsVector.push_back((uint8_t)((sixtyFourBitInt & 0x000000000000FF00) >> 8));
	eightBitIntsVector.push_back((uint8_t)(sixtyFourBitInt & 0x00000000000000FF));
	return eightBitIntsVector;
}


void SatUI::MyForm::insertSixteenBitIntInEightBitVector(std::vector<uint8_t> & eightBitIntsVector, std::vector<uint8_t>::iterator position, uint16_t sixteenBitInt) {
	std::vector<uint8_t> vectorToBeInserted = splitSixteenBitInt(sixteenBitInt);
	eightBitIntsVector.insert(position, vectorToBeInserted.begin(), vectorToBeInserted.end());
}

void SatUI::MyForm::insertThirtyTwoBitIntInEightBitVector(std::vector<uint8_t> & eightBitIntsVector, std::vector<uint8_t>::iterator position, uint32_t thirtyTwoBitInt) {
	std::vector<uint8_t> vectorToBeInserted = splitThirtyTwoBitInt(thirtyTwoBitInt);
	eightBitIntsVector.insert(position, vectorToBeInserted.begin(), vectorToBeInserted.end());
}

void SatUI::MyForm::insertSixtyFourBitIntInEightBitVector(std::vector<uint8_t> & eightBitIntsVector, std::vector<uint8_t>::iterator position, uint64_t sixtyFourBitInt) {
	std::vector<uint8_t> vectorToBeInserted = splitSixtyFourBitInt(sixtyFourBitInt);
	eightBitIntsVector.insert(position, vectorToBeInserted.begin(), vectorToBeInserted.end());
}


uint16_t SatUI::MyForm::makeSixteenBitInt(std::vector<uint8_t> eightBitIntsVector) {
	uint16_t sixteenBitInt = 0x0000;
	int vectorSize = eightBitIntsVector.size();
	if (vectorSize != 2)
		log("Note (possible Error): Converting vector of size " + std::to_string(vectorSize) + " to uint16_t.");
	for (int i = 0; ((i < 2) && (i < vectorSize)); i++) {
		sixteenBitInt += (((uint16_t)eightBitIntsVector[i]) << (8 - (i * 8)));
	}
	return sixteenBitInt;
}

uint32_t SatUI::MyForm::makeThirtyTwoBitInt(std::vector<uint8_t> eightBitIntsVector) {
	uint32_t thirtyTwoBitInt = 0x00000000;
	int vectorSize = eightBitIntsVector.size();
	if (vectorSize != 4)
		log("Note (possible Error): Converting vector of size " + std::to_string(vectorSize) + " to uint32_t.");
	for (int i = 0; ((i < 4) && (i < vectorSize)); i++) {
		thirtyTwoBitInt += (((uint32_t)eightBitIntsVector[i]) << (24 - (i * 8)));
	}
	return thirtyTwoBitInt;
}

uint64_t SatUI::MyForm::makeSixtyFourBitInt(std::vector<uint8_t> eightBitIntsVector) {
	uint64_t sixtyFourBitInt = 0x0000000000000000;
	int vectorSize = eightBitIntsVector.size();
	if (vectorSize != 8)
		log("Note (possible Error): Converting vector of size " + std::to_string(vectorSize) + " to uint64_t.");
	for (int i = 0; ((i < 8) && (i < vectorSize)); i++) {
		sixtyFourBitInt += (((uint64_t)eightBitIntsVector[i]) << (56 - (i * 8)));
	}
	return sixtyFourBitInt;
}


uint16_t SatUI::MyForm::getSixteenBitIntFromEightBitVector(std::vector<uint8_t> & eightBitIntsVector, int start) {
	int vectorSize = eightBitIntsVector.size();
	std::vector<uint8_t> vectorOfSixteenBitInt;
	if (start >= vectorSize) {
		log("Note (possible Error): Converting out of range part of vector to uint16_t.");
		return 0x0000;
	}
	int end = start + 2;
	if (end > vectorSize) {
		end = vectorSize;
	}
	vectorOfSixteenBitInt.insert(vectorOfSixteenBitInt.begin(), eightBitIntsVector.begin() + start, eightBitIntsVector.begin() + end);
	return makeSixteenBitInt(vectorOfSixteenBitInt);
}

uint32_t SatUI::MyForm::getThirtyTwoBitIntFromEightBitVector(std::vector<uint8_t> & eightBitIntsVector, int start) {
	int vectorSize = eightBitIntsVector.size();
	std::vector<uint8_t> vectorOfThirtyTwoBitInt;
	if (start >= vectorSize) {
		log("Note (possible Error): Converting out of range part of vector to uint32_t.");
		return 0x00000000;
	}
	int end = start + 4;
	if (end > vectorSize) {
		end = vectorSize;
	}
	vectorOfThirtyTwoBitInt.insert(vectorOfThirtyTwoBitInt.begin(), eightBitIntsVector.begin() + start, eightBitIntsVector.begin() + end);
	return makeThirtyTwoBitInt(vectorOfThirtyTwoBitInt);
}

uint64_t SatUI::MyForm::getSixtyFourBitIntFromEightBitVector(std::vector<uint8_t> & eightBitIntsVector, int start) {
	int vectorSize = eightBitIntsVector.size();
	std::vector<uint8_t> vectorOfSixtyFourBitInt;
	if (start >= vectorSize) {
		log("Note (possible Error): Converting out of range part of vector to uint64_t.");
		return 0x00000000;
	}
	int end = start + 8;
	if (end > vectorSize) {
		end = vectorSize;
	}
	vectorOfSixtyFourBitInt.insert(vectorOfSixtyFourBitInt.begin(), eightBitIntsVector.begin() + start, eightBitIntsVector.begin() + end);
	return makeSixtyFourBitInt(vectorOfSixtyFourBitInt);
}


std::string SatUI::MyForm::vectorToHexString(std::vector<uint8_t> & vectorOfEightBitInts) {
	std::stringstream ss;
	if (vectorOfEightBitInts.size() == 0) {
		return "";
	}
	char val;
	int test;
	for (unsigned int i = 0; i < vectorOfEightBitInts.size(); i++) {
		test = (vectorOfEightBitInts[i] & 0xF0) >> 4;
		if (test >= 10) val = (test - 10) + 'A';
		else val = test + '0';
		ss << " " << val;
		test = vectorOfEightBitInts[i] & 0x0F;
		if (test >= 10) val = (test - 10) + 'A';
		else val = test + '0';
		ss << val;
	}
	return ss.str();
}
std::string SatUI::MyForm::integerToHexString(uint32_t integer, int hexDigits) {
	std::stringstream ss;
	ss << "0x";
	char val;
	uint32_t test;
	for (int i = (hexDigits-1); i >= 0; i--) {
		test = ((integer >> (i * 4)) & 0x0000000F);
		if (test >= 10) val = (test - 10) + 'A';
		else val = test + '0';
		ss << val;
	}
	return ss.str();
}

#endif // !BIT_MANIPULATION_H
