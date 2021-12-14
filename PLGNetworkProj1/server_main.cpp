#define WIN32_LEAN_AND_MEAN			// Strip rarely used calls

//server
//Anthony Christopher X Michael Larramee
//12/10/21
//server_main.cpp

// I used LM5 AND LM6 FROM REFERENCING
// Resources:https://www.binarytides.com/winsock-socket-programming-tutorial/
//https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancedcode1d.html


#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <Buffer.h>
#include <ProtocolHelper.h>

// Links library ws2.32
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

Buffer outgoing(DEFAULT_BUFLEN);
Buffer ingoing(DEFAULT_BUFLEN);

//Strcuture:Client
struct infoOfClient
{
	SOCKET socket;

	// Buffer information (this is basically you buffer class)
	WSABUF dataBuf;
	char buffer[DEFAULT_BUFLEN];
	int bytesRECV;
};

int ClientTotal = 0;
infoOfClient* ClientArray[FD_SETSIZE];

void ClientRemove(int index)
{
	infoOfClient* client = ClientArray[index];
	closesocket(client->socket);
	printf("Closing socket %d\n", (int)client->socket);

	for (int clientIndex = index; clientIndex < ClientTotal; clientIndex++)
	{
		ClientArray[clientIndex] = ClientArray[clientIndex + 1];
	}

	ClientTotal--;

	// Cleans up
	// Deletes Client
	delete client;
}

int main(int argc, char** argv)
{
	WSADATA wsaData;
	int iResult;

	// Creates Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		// error id
		printf("WSAStartup failed ERROR: %d\n", iResult);
		return 1;
	}
	else
	{
		printf("WSAStartup()  SUCCESFUL!\n");
	}

	// Socket #1
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET acceptSocket = INVALID_SOCKET;

	struct addrinfo* addrResult = NULL;
	struct addrinfo hints;

	// Defines connection
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	//Server address resolve
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addrResult);
	if (iResult != 0)
	{
		printf("getaddrinfo() failed ERROR: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("getaddrinfo() is good!\n");
	}

	// Creates socket for connecting to server
	listenSocket = socket(
		addrResult->ai_family,
		addrResult->ai_socktype,
		addrResult->ai_protocol
	);
	if (listenSocket == INVALID_SOCKET)
	{
		// Real error code
		// https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
		printf("socket() failed ERROR: %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("Created socket()!\n");
	}

	// #2 Binds and creates listening
	iResult = bind(
		listenSocket,
		addrResult->ai_addr,
		(int)addrResult->ai_addrlen
	);
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed ERROR: %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("bind() is GOOD!\n");
	}

	// Not needed
	freeaddrinfo(addrResult);

	// #3 Listening
	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen() failed ERROR: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("listen() SUCCESFUL!\n");
	}

	// Socket mode from blocking to non block so requests arent blocked
	DWORD NonBlock = 1;
	iResult = ioctlsocket(listenSocket, FIONBIO, &NonBlock);
	if (iResult == SOCKET_ERROR)
	{
		printf("ioctlsocket() failed ERROR: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	printf("ioctlsocket() SUCCESFUL!\n");

	FD_SET ReadSet;
	int total;
	
	DWORD RecvBytes;
	DWORD SentBytes;

	printf("Entering Accept/Recieve/Sending loop...\n");
	while (true)
	{
		timeval tv = { 0 };
		tv.tv_sec = 2;
		// Initialize our read set
		FD_ZERO(&ReadSet);

		// Always look for connection attempts
		FD_SET(listenSocket, &ReadSet);

		// Set read notification for each socket.
		for (int i = 0; i < ClientTotal; i++)
		{
			FD_SET(ClientArray[i]->socket, &ReadSet);
		}

		// Call our select function to find the sockets that
		// require our attention
		printf("select() Loading.....\n");
		total = select(0, &ReadSet, NULL, NULL, &tv);
		if (total == SOCKET_ERROR)
		{
			printf("select() failed ERROR: %d\n", WSAGetLastError());
			return 1;
		}
		else
		{
			printf("select() SUCCESSFUL!\n");
		}

		// #4 Incoming connections checks
		if (FD_ISSET(listenSocket, &ReadSet))
		{
			total--;
			acceptSocket = accept(listenSocket, NULL, NULL);
			if (acceptSocket == INVALID_SOCKET)
			{
				printf("accept() failed ERROR: %d\n", WSAGetLastError());
				return 1;
			}
			else
			{
				iResult = ioctlsocket(acceptSocket, FIONBIO, &NonBlock);
				if (iResult == SOCKET_ERROR)
				{
					printf("ioctsocket() failed ERROR: %d\n", WSAGetLastError());
				}
				else
				{
					printf("ioctlsocket() SUCCESS!\n");

					infoOfClient* info = new infoOfClient();
					info->socket = acceptSocket;
					info->bytesRECV = 0;
					ClientArray[ClientTotal] = info;
					ClientTotal++;
					printf("New client connected on socket %d\n", (int)acceptSocket);
				}
			}
		}

		// #5 RECIEVE & SEND
		for (int i = 0; i < ClientTotal; i++)
		{
			infoOfClient* client = ClientArray[i];

			// when readset is marked, data is available
			
			if (FD_ISSET(client->socket, &ReadSet))
			{
				total--;
				client->dataBuf.buf = client->buffer;
				client->dataBuf.len = DEFAULT_BUFLEN;

				DWORD Flags = 0;
				iResult = WSARecv(
					client->socket,
					&(client->dataBuf),
					1,
					&RecvBytes,
					&Flags,
					NULL,
					NULL
				);



				std::string received = "";

				for (DWORD i = 0; i < RecvBytes; i++) {
					received.push_back(client->dataBuf.buf[i]);
				}

				ingoing.LoadBuffer(received);

				sProtocolData data = ProtocolMeths::ParseBuffer(ingoing);

				//Creates buffer for the messages
				if (data.type == ROOM_JOIN)
				{
					outgoing = ProtocolMeths::MakeProtocol(RECV_MESSAGE, "Server", data.room, data.message);

				}
				else if (data.type == MESSAGE_SEND)
				{
					outgoing = ProtocolMeths::MakeProtocol(RECV_MESSAGE, data.userName, data.room, data.message);
				}
				else if (data.type == EXIT_ROOM)
				{
					outgoing = ProtocolMeths::MakeProtocol(RECV_MESSAGE, "Server", data.room, data.message);
				}

				
				std::cout << "RECEIEVED: " << received << std::endl;

				if (iResult == SOCKET_ERROR)
				{
					if (WSAGetLastError() == WSAEWOULDBLOCK)
					{
						// Not as important
					}
					else
					{
						printf("WSARecv failed on socket %d ERROR: %d\n", (int)client->socket, WSAGetLastError());
						ClientRemove(i);
					}
				}
				else
				{
					printf("WSARecv() is OK!\n");
					if (RecvBytes == 0)
					{
						ClientRemove(i);
					}
					else if (RecvBytes == SOCKET_ERROR)
					{
						printf("RECEIVE: ERROR..%d\n", WSAGetLastError());
						continue;
					}
					else
					{
						char* payload = outgoing.PayloadToString();
						WSABUF buf;
						buf.buf = payload;
						buf.len = outgoing.readUInt32BE(0);

						for (int i = 0; i < ClientTotal; i++)
						{
							iResult = WSASend(
								ClientArray[i]->socket,
								&(buf), 
								1,
								&SentBytes,
								Flags,
								NULL,
								NULL
							);

							

							if (SentBytes == SOCKET_ERROR)
							{
								printf("Sending error %d\n", WSAGetLastError());
							}
							else if (SentBytes == 0)
							{
								printf("Result send is 0\n");
							}
							else
							{
								printf("Sent SUCCESFULLY %d bytes!\n", SentBytes);
							}
						}
						delete[] payload; //cleans loadpay
					}
				}
			}
		}
	}

	// #6 Closing
	iResult = shutdown(acceptSocket, SD_SEND);
	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed ERROR: %d\n", WSAGetLastError());
		closesocket(acceptSocket);
		WSACleanup();
		return 1;
	}

	// Cleaning
	closesocket(acceptSocket);
	WSACleanup();

	return 0;
}