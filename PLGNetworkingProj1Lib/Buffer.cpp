//Buffer.cpp
//Anthony Christopher X Michael Larramee
//12/10/21
//  Method:Manifest in header

#include "Buffer.h"
// Resource: https://stackoverflow.com/questions/143123/how-to-create-a-buffer-for-reading-socket-data-in-c 
Buffer::Buffer(size_t size) 
{
	buffer = std::vector<uint8_t>(size);
	readIndex = 0;
	writeTheIndex = 0;
}

void Buffer::writeUInt32BE(std::size_t index, int32_t value) 
{
	buffer.insert(buffer.begin() + index, value >> 24);
	buffer.insert(buffer.begin() + index + 1, value >> 16);
	buffer.insert(buffer.begin() + index + 2, value >> 8);
	buffer.insert(buffer.begin() + index + 3, value);

	writeTheIndex += 4;
}

void Buffer::writeUInt32BE(int32_t value) 
{
	if (writeTheIndex >= buffer.size())
	{
		buffer.resize(writeTheIndex + 4);
	}

	buffer[writeTheIndex] = value >> 24;
	buffer[writeTheIndex + 1] = value >> 16;
	buffer[writeTheIndex + 2] = value >> 8;
	buffer[writeTheIndex + 3] = value;

	writeTheIndex += 4;
}

uint32_t Buffer::readUInt32BE(std::size_t index) 
{
	uint32_t value = buffer[index] << 24;
	value |= buffer[index + 1] << 16;
	value |= buffer[index + 2] << 8;
	value |= buffer[index + 3];

	return value;
}

uint32_t Buffer::readUInt32BE() 
{
	uint32_t value = buffer[readIndex] << 24;
	value |= buffer[readIndex + 1] << 16;
	value |= buffer[readIndex + 2] << 8;
	value |= buffer[readIndex + 3];

	readIndex += 4;
	return value;
}

void Buffer::writeUInt16BE(std::size_t index, int16_t value)
{

	buffer.insert(buffer.begin() + index, value >> 8);
	buffer.insert(buffer.begin() + index + 1, value);

	writeTheIndex += 2;
}

void Buffer::writeUInt16BE(int16_t value)
{
	if (writeTheIndex >= buffer.size())
	{
		buffer.resize(writeTheIndex + 2);
	}
	buffer[writeTheIndex] = value >> 8;
	buffer[writeTheIndex + 1] = value;

	writeTheIndex += 2;
}

uint16_t Buffer::readUInt16BE(std::size_t index)
{
	uint16_t value = buffer[index] << 8;
	value |= buffer[index + 1];

	return value;
}
//-E
uint16_t Buffer::readUInt16BEE()
{
	uint16_t value = buffer[readIndex] << 8;
	value |= buffer[readIndex + 1];

	readIndex += 2;
	return value;
}

void Buffer::writeUInt8BE(std::size_t index, std::string value)
{
	int charOrder = 0;
	for (char c : value)
	{
		//Chatorder+index
		buffer.insert(buffer.begin() + index + charOrder, c);
		charOrder++;
	}

	writeTheIndex += charOrder;
}

void Buffer::writeUInt8BE(std::string value)
{
	if (writeTheIndex >= buffer.size())
	{
		buffer.resize(writeTheIndex + value.length());
	}

	int charOrder = 0;
	for (char c : value)
	{
		buffer[writeTheIndex + charOrder] = c;
		charOrder++;
	}

	writeTheIndex += charOrder;
}

std::string Buffer::readUInt8BE(std::size_t index, int stringSize)
{
	std::string output;
	for (int i = 0; i < stringSize; i++)
	{
		output.push_back(buffer[index + i]);
	}

	return output;
}

std::string Buffer::readUInt8BE(int stringSize)
{
	std::string output = "";
	for (int i = 0; i < stringSize; i++)
	{
		output.push_back(buffer[readIndex]);
		readIndex++;
	}

	return output;
}

std::vector<uint8_t> Buffer::GetBuffer()
{
	return buffer;
}

int Buffer::GetWriteIndex()
{
	return writeTheIndex;
}

char* Buffer::PayloadToString() {
	int totalLength = readUInt32BE();
	char* outbound = new char[totalLength];
	for (int i = 0; i < totalLength; i++) {
		outbound[i] = buffer[i]; 
	}
	return outbound;
}

void Buffer::LoadBuffer(std::string recvd)
{
	ClearBuffer();
	for (char c : recvd)
	{
		buffer.push_back(c);
	}
}

void Buffer::ClearBuffer()
{
	buffer.clear();
	readIndex = 0;
	writeTheIndex = 0;
}
