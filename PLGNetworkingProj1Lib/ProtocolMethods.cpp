#include "ProtocolHelper.h"

//ProtocolMethods.cpp
//Anthony Christopher X Michael Larramee
//12/10/21
//Protocol utility methods


//Protocol structure
/*
	ROOM_JOIN :  header/len/name/len/roomName/
	EXIT_ROOM:  header/len/name/len/roomName/

*/

Buffer ProtocolMeths::MakeProtocol(TypeProtocol type, std::string name, std::string room, std::string message)
{
	Buffer tempBuf(DEFAULT_BUFLEN);

	if (type == ROOM_JOIN)
	{
		tempBuf.writeUInt32BE(name.length());
		tempBuf.writeUInt8BE(name);

		tempBuf.writeUInt32BE(room.length());
		tempBuf.writeUInt8BE(room);

		//length of ALL
		int length = tempBuf.GetWriteIndex() + 8;
		tempBuf.writeUInt32BE(0, length);

		tempBuf.writeUInt32BE(4, ROOM_JOIN);

		
	}
	else if (type == EXIT_ROOM)
	{
		tempBuf.writeUInt32BE(name.length());
		tempBuf.writeUInt8BE(name);

		tempBuf.writeUInt32BE(room.length());
		tempBuf.writeUInt8BE(room);

		//length of ALL
		int length = tempBuf.GetWriteIndex() + 8;
		tempBuf.writeUInt32BE(0, length);

		tempBuf.writeUInt32BE(4, EXIT_ROOM);

	}
	else if (type == MESSAGE_SEND)
	{
		tempBuf.writeUInt32BE(name.length());
		tempBuf.writeUInt8BE(name);

		tempBuf.writeUInt32BE(room.length());
		tempBuf.writeUInt8BE(room);

		tempBuf.writeUInt32BE(message.length());
		tempBuf.writeUInt8BE(message);

		int length = tempBuf.GetWriteIndex() + 8;
		tempBuf.writeUInt32BE(0, length);

		tempBuf.writeUInt32BE(4, MESSAGE_SEND);
	}
	else if (type == RECV_MESSAGE) 
	{
		tempBuf.writeUInt32BE(name.length());
		tempBuf.writeUInt8BE(name);

		tempBuf.writeUInt32BE(room.length());
		tempBuf.writeUInt8BE(room);

		tempBuf.writeUInt32BE(message.length());
		tempBuf.writeUInt8BE(message);

		int length = tempBuf.GetWriteIndex() + 8;
		tempBuf.writeUInt32BE(0, length);

		tempBuf.writeUInt32BE(4, RECV_MESSAGE);
	}

	return tempBuf;
}

sProtocolData ProtocolMeths::ParseBuffer(Buffer input)
{
	sProtocolData data;
	if (input.GetBuffer().size() == 0) //size check
	{
		return data;
	}
	
	int length;
	length = input.readUInt32BE();
	data.type = (TypeProtocol)input.readUInt32BE();

	if (data.type == ROOM_JOIN)
	{
		int itemLength = input.readUInt32BE();
		data.userName = input.readUInt8BE(itemLength);

		itemLength = input.readUInt32BE();
		data.room = input.readUInt8BE(itemLength);

		//Create the message
		data.message = data.userName + " entered " + data.room + " room";
	}
	else if (data.type == EXIT_ROOM)
	{
		int lengthOfItem = input.readUInt32BE();
		data.userName = input.readUInt8BE(lengthOfItem);

		lengthOfItem = input.readUInt32BE();
		data.room = input.readUInt8BE(lengthOfItem);

		//construct the message
		data.message = data.userName + " left the " + data.room + " room";
	}
	else if (data.type == MESSAGE_SEND) 
	{
		uint32_t lengthOfName = input.readUInt32BE();
		data.userName = input.readUInt8BE(lengthOfName);
		uint32_t lengthOfRoom = input.readUInt32BE();
		data.room = input.readUInt8BE(lengthOfRoom);
		uint32_t lengthOfMessage = input.readUInt32BE();
		data.message = input.readUInt8BE(lengthOfMessage);
	}
	else if (data.type == RECV_MESSAGE)
	{

		uint32_t nameLength = input.readUInt32BE();
		data.userName = input.readUInt8BE(nameLength);
		uint32_t roomLength = input.readUInt32BE();
		data.room = input.readUInt8BE(roomLength);
		uint32_t msgLength = input.readUInt32BE();
		data.message = input.readUInt8BE(msgLength);
	}

	return data;
}