//Buffer.h
//Anthony Christopher X Matthew Laramme 
//12/09/2021
//tcp packet buffer use

#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>



class Buffer {
private:
	int readIndex;
	int writeTheIndex;
	std::vector<uint8_t> buffer;

public:
	//constructor
	Buffer(std::size_t size);

	//Will read and write
	void writeUInt32BE(std::size_t index, int32_t value);
	void writeUInt32BE(int32_t value);
	uint32_t readUInt32BE(std::size_t index);
	uint32_t readUInt32BE();

	//Will read and write
	void writeUInt16BE(std::size_t index, int16_t value);
	void writeUInt16BE(int16_t value);
	uint16_t readUInt16BE(std::size_t index);
	uint16_t readUInt16BEE();

	//Will read and write
	void writeUInt8BE(std::size_t index, std::string value);
	void writeUInt8BE(std::string value);
	std::string readUInt8BE(std::size_t index, int stringSize);
	std::string readUInt8BE(int stringSize);

	//buffer getter
	std::vector<uint8_t> GetBuffer();
	//index getter
	int GetWriteIndex();

	//turns buffer into char array
	char* PayloadToString();

	//for buffer loading
	void LoadBuffer(std::string);

	//flushing/clearing
	void ClearBuffer();
};
