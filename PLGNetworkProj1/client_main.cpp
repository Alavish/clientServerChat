#define WIN32_LEAN_AND_MEAN

//client
//Anthony Christopher X Michael Larramee
//12/10/21
//client_main.cpp

//Resources:https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancedcode1d.html
//https://www.youtube.com/watch?v=wKxbjB6zqS8

#include <windows.h>
#include <winsock2.h>
#include <conio.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <Buffer.h>
#include <ProtocolHelper.h>


// Links the librarys
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512						// Buffer length default
#define DEFAULT_PORT "27015"					// port default
#define SERVER "127.0.0.1"						// IP Address of the server

Buffer outgoing(DEFAULT_BUFLEN);
Buffer incoming(DEFAULT_BUFLEN);

//Sends to client for acception or decline
std::vector<std::string> rooms;


int main(int argc, char **argv)
{
	WSADATA wsaData;							// Keeps Winsock data
	SOCKET connectSocket = INVALID_SOCKET;		// Connection socket

	struct addrinfo *resultInfo = NULL;			// address info held here
	struct addrinfo *ptr = NULL;
	struct addrinfo hints;
	u_long mode = 1;

	// Message to sent server
	std::string message = "";
	std::vector<std::string> chatlog;

	char recvbuf[DEFAULT_BUFLEN];				// MAX buffer size
	int result;									// result for commands
	int recvbuflen = DEFAULT_BUFLEN;			// length of the buffer

	// Step #1 Create Winsock
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("WSAStartup failed with error: %d\n", result);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Step #2 Resolve the server address and port
	result = getaddrinfo(SERVER, DEFAULT_PORT, &hints, &resultInfo);
	if (result != 0)
	{
		printf("getaddrinfo failed ERROR: %d\n", result);
		WSACleanup();
		return 1;
	}

	bool isConnected = false;
	// Step #3 Try connecting address 
	for (ptr = resultInfo; ptr != NULL; ptr = ptr->ai_next)
	{
		// Makes socket for connection
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (connectSocket == INVALID_SOCKET)
		{
			printf("socket failed ERROR: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}


		// Connect to server.
		result = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (result == SOCKET_ERROR)
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}

		result = ioctlsocket(connectSocket, FIONBIO, &mode);
		if (result != NO_ERROR) {
			printf("ioctlsocket failed ERROR: %ld\n", result);
			
		}
			isConnected = true;
		break;
	}

	freeaddrinfo(resultInfo);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("Cant Connect To Server\n");
		WSACleanup();
		return 1;
	}

	//Get logging information
	std::string username = "";
	std::string nameOfRoom = "";
	std::cout << "Enter your username: ";
	std::cin >> username;
	std::cout << "Enter name of the room you want to join: ";
	std::cin >> nameOfRoom;

	//Creates the protocol
	outgoing = ProtocolMeths::MakeProtocol(ROOM_JOIN, username, nameOfRoom, "");//this has no inherent message
	sProtocolData data = ProtocolMeths::ParseBuffer(outgoing);

	//Changes format for manipulation
	char* loadPay = outgoing.PayloadToString();
	//it sends
	result = send(connectSocket, loadPay, outgoing.readUInt32BE(0), 0);
	if (result == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	rooms.push_back(nameOfRoom);

	//cleans
	delete[] loadPay;
	printf("Bytes Sent: %ld\n", result);
	
	//client loop starts
	bool logUpdate = false;
	bool quit = false;
	bool requestedHelp = false;
	bool commandInvalid = false;
	while (!quit) {

		//input keyboard
		if (_kbhit()) {
			char key = _getch();

			if (key == 27) { //use esc for quitting
				quit = true;
			}else if (key == 8) { //go back to remove
				if (message.length() != 0)
				{
					message.pop_back();
					system("cls"); //Clears the system not the best practice, I was having trouble without it and other ways werent properly working
					logUpdate = true;
				}
			}
			else if (key == 13) { // enter for sending

				requestedHelp = false; //removes help
				
				if (message[0] == '/') // command
				{
					size_t pos = message.find(" ");
					std::string command = message.substr(0, pos);
					message.erase(0, pos + 1);

					if (command == "/help" || command == "/h") {
						requestedHelp = true;
						logUpdate;
					}else if (command == "/join" || command == "/j")
					{
						if (std::count(rooms.begin(), rooms.end(), message))
						{
							chatlog.push_back("You are already in the room " + message);
						}
						else
						{
							//protocol assembling
							outgoing = ProtocolMeths::MakeProtocol(ROOM_JOIN, username, message, "");//this has no  message
							sProtocolData data = ProtocolMeths::ParseBuffer(outgoing);

							//change it to a format we can transport
							char* loadPay = outgoing.PayloadToString();
							//send it
							result = send(connectSocket, loadPay, outgoing.readUInt32BE(0), 0);
							if (result == SOCKET_ERROR)
							{
								printf("send failed ERROR: %d\n", WSAGetLastError());
								closesocket(connectSocket);
								WSACleanup();
								return 1;
							}

							rooms.push_back(message);

							//cleans
							delete[] loadPay;
							printf("Bytes Sent: %ld\n", result);
						}
					}
					else if (command == "/message" || command == "/m")
					{
						pos = message.find(" ");
						std::string room = message.substr(0, pos);
						message.erase(0, pos + 1);

						if (std::count(rooms.begin(), rooms.end(), room) == 0)
						{
							chatlog.push_back("Currently you arent in the room " + room);
						}
						else
						{
							//call to asseble the protocol
							outgoing = ProtocolMeths::MakeProtocol(MESSAGE_SEND, username, room, message);
							

							//change it to a form we can transport
							char* payload = outgoing.PayloadToString();
							//send it
							result = send(connectSocket, payload, outgoing.readUInt32BE(0), 0);

							if (result == SOCKET_ERROR)
							{
								printf("send failed ERROR: %d\n", WSAGetLastError());
								closesocket(connectSocket);
								WSACleanup();
								return 1;
							}

							//clean up
							delete[] payload;
							printf("Bytes Sent: %ld\n", result);
						}
					}
					else if (command == "/leave" || command == "/l")
					{
						if (std::count(rooms.begin(), rooms.end(), message) == 0)
						{
							chatlog.push_back("Currently you're not in the room " + message);
						}
						else
						{
							//Leave
							outgoing = ProtocolMeths::MakeProtocol(EXIT_ROOM, username, message, "");

							//changes format for transfer
							char* leavePayload = outgoing.PayloadToString();
							//send it
							result = send(connectSocket, leavePayload, outgoing.readUInt32BE(0), 0);
							if (result == SOCKET_ERROR)
							{
								printf("send failed ERROR: %d\n", WSAGetLastError());
								closesocket(connectSocket);
								WSACleanup();
								return 1;
							}

							//clean up
							delete[] leavePayload;
							printf("Bytes Sent: %ld\n", result);
							rooms.erase(std::find(rooms.begin(), rooms.end(), message));
						}
					}
				}
				else{ commandInvalid = true; }
				message = "";
				logUpdate = true;
			}
			else {
				message.push_back(key);
				system("cls"); //Not best thing to do but it clears
				logUpdate = true;				
			}
		}

		if (isConnected) {
			result = recv(connectSocket, recvbuf, recvbuflen, 0);
			if (result > 0)
			{
				
				//Gets incoming message
				printf("Bytes received: %d\n", result);
				std::string received = "";
				for (int i = 0; i < recvbuflen; i++) {
					received.push_back(recvbuf[i]);
				}

				incoming.LoadBuffer(received);

				//Parses

				sProtocolData data = ProtocolMeths::ParseBuffer(incoming);

				//if from same room show in chat
				system("cls"); 
				for (int i = 0; i < rooms.size(); i++) {
					if (rooms[i] == data.room) { 
						chatlog.push_back("<" + data.room + "> " + data.userName + ":\t" + data.message);
					}
					logUpdate = true;
				}
			}
			else if (result == 0)
			{
				//if client leaves
				printf("Connection closed\n");
			}
			else
			{
				if (WSAGetLastError() == 10035) {
					//nothing recieved
				}
				else {
					printf("Message error......EXITING");
					quit = true;
				}
			}

			//Prints chat log
			if (logUpdate) {
				for (int i = 0; i < chatlog.size(); i++) {
					std::cout << chatlog[i] << std::endl;
				}
				std::cout << std::endl;
				//User feedback
				if (commandInvalid) {
					printf("\x1B[91m%s\033[0m\n", "!Invalid Command!");
					commandInvalid = false;
				}
				if (requestedHelp) {
					//Command list print
					std::cout << "/help: Shows list of commands" << std::endl;
					std::cout << "/join [room_name]: Joins a room" << std::endl;
					std::cout << "/message [room_name]: Sends message to room" << std::endl;
					std::cout << "/leave [room_name]: Leaves the room" << std::endl;
					
					
				}
				else {
					std::cout << "type /help for a list of commands" << std::endl;
				}
				//for user inputting
				std::cout << "message: ";
				std::cout << message << std::endl;
				logUpdate = false;
			}

		}

	}

	for (int i = 0; i < rooms.size(); i++)
	{
		//Leave
		outgoing = ProtocolMeths::MakeProtocol(EXIT_ROOM, username, rooms[i], "");//this has no inherent message
		sProtocolData leaveData = ProtocolMeths::ParseBuffer(outgoing);

		//changing format for transfer
		char* leavePayload = outgoing.PayloadToString();
		//sending
		result = send(connectSocket, leavePayload, outgoing.readUInt32BE(0), 0);
		if (result == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

		//cleans
		delete[] leavePayload;
		printf("Bytes Sent: %ld\n", result);
	}

	// Step #5 Closing connection because no more needed data
	result = shutdown(connectSocket, SD_SEND);
	if (result == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	// Step #7 clean
	closesocket(connectSocket);
	WSACleanup();

	system("pause");

	return 0;
}