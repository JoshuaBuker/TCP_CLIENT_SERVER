#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:4996) 

#define MessageLength 1024
#define MaxClients 10

int RecvMessages(SOCKET Socket, std::vector<SOCKET> Clients) {
	// Get The Message
	char Message[MessageLength];
	int Bytes = recv(Socket, Message, MessageLength, 0);
	int MessageLen = sizeof(Message);

	// Send The Message To All OTher Clients
	if (Bytes != 0) {
		std::cout << "Client: " << Message << std::endl;
		for (SOCKET Client : Clients) {
			if (Socket != Client) {
				send(Socket, Message, MessageLen, 0);
			}
		}
	}

	return 0;
}

SOCKET NewSocket() { // This Being In A Function Is Now Pointless, But Can Be Left Alone
	// Create A Socket
	SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET) {
		std::cout << "Socket Failed With Error: " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	} else {
		std::cout << "Connection Succesful" << std::endl;
	}

	return Socket;
}

void ConnectionManager() { // Put In Thread
	std::cout << "Listening For Connection's" << std::endl;

	// Get The Number Of Clients In The Client Vector
	std::vector<SOCKET> Clients(MaxClients);
	int ClientCount;

	// Listen For Connections
	while (true) {
		SOCKET Socket = INVALID_SOCKET;
		SOCKET Client = accept(Socket, NULL, NULL);
		Clients.push_back(Client);

		// Get Messages From Client
		int Status = RecvMessages(Socket, Clients);
		if (Status == 0) {
			continue;

		} else {
			std::cout << "Connection Suddenly Stopped" << std::endl;
			std::terminate(); // STOP THREAD
		}
	}
}

int main() {
	// Initialize WSA
	int WSAResult;
	WSADATA WSAData;

	SOCKET connect = INVALID_SOCKET;

	WSAResult = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (WSAResult != NO_ERROR) {
		std::cout << "WSA Startup With Error: " << WSAResult << std::endl;
		return 1;
	}

	// Setup the Socket Information
	struct sockaddr_in SockInfo;
	SockInfo.sin_family = AF_INET;
	SockInfo.sin_addr.s_addr = inet_addr("127.0.0.1");
	SockInfo.sin_port = htons(55555);

	// Manage The Connection
	std::thread obj(ConnectionManager);
	obj.join();

	return 0;
}