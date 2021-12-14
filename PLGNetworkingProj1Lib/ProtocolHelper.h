#pragma once

//ProtocolHelper.h
//Anthony Christopher X Michael Larramee
//12/10/21
//Outlines our protocols

#define DEFAULT_BUFLEN 512

#include <string>
#include "Buffer.h"

enum TypeProtocol
{
	ROOM_JOIN = 1,
	EXIT_ROOM,
	MESSAGE_SEND,
	RECV_MESSAGE
};

//data struct of protocol
struct sProtocolData
{
	TypeProtocol type;
	std::string userName;
	std::string room;
	std::string message;
};


class ProtocolMeths
{
	ProtocolMeths() = delete;

public:
	//protocol data to create buffer
	static Buffer MakeProtocol(TypeProtocol type, std::string name, std::string room, std::string message);

	//Gets data from buffer and converts
	static sProtocolData ParseBuffer(Buffer input);
};




