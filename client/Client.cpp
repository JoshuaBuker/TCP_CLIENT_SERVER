#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>

#pragma comment(lib,"Ws2_32.lib")
//using std::cout, std::string;

#define PORT 55555
#define IP "127.0.0.1"
#define MAX_MESSAGE_LENGTH 1024

SOCKET clientSocket;
std::unique_ptr<std::thread> threads[2];

int getMessages(const SOCKET& clientSocket) {
  char buffer[MAX_MESSAGE_LENGTH];

  while (true) {
    int byteCount = recv(clientSocket, buffer, MAX_MESSAGE_LENGTH, 0);
    if (byteCount > 0) {
      std::cout << "\nOther User: " << buffer << "\n";
    } 
  }

  return 0;
}

int sendMessages(const SOCKET& clientSocket) {
  char buffer[MAX_MESSAGE_LENGTH];

  while (true) {
    std::cin.getline(buffer, MAX_MESSAGE_LENGTH);
    send(clientSocket, buffer, MAX_MESSAGE_LENGTH, 0);
  }

  return 0;
}

int loadDLL() {
  WSADATA wsaData;
  int wsaerr;
  WORD wVersionRequested = MAKEWORD(2, 2);

  wsaerr = WSAStartup(wVersionRequested, &wsaData);

  if (wsaerr != 0) {
      std::cout << "The winsock dll not found!" << std::endl;
    return -1;
  }

  return 0;
}

int createSocket(SOCKET& clientSocket) {
  clientSocket = INVALID_SOCKET;

  clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (clientSocket == INVALID_SOCKET) {
      std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
    WSACleanup();
    return -1;
  }

  return 1;
}

int connectToServer(const SOCKET& clientSocket) {
  sockaddr_in clientService;
  clientService.sin_family = AF_INET;

  InetPton(AF_INET, _T(IP), &clientService.sin_addr.s_addr);

  clientService.sin_port = htons(PORT);

  if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
    std::cout << "Client: connect() - Failed To Connect: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return -1;
  }

  return 0;
}

int initializeServer(SOCKET& clientSocket) {
  if (loadDLL() == -1) { return -1; }
  if (createSocket(clientSocket) == -1) { return -1; };
  if (connectToServer(clientSocket) == -1) { return -1; };

  return 0;
}

int main(void) {
  if (initializeServer(clientSocket) == -1) {
    std::cout << "Failed to initalize server.";
    return 0;
  }

  threads[0] = std::make_unique<std::thread>(sendMessages, std::ref(clientSocket));
  threads[1] = std::make_unique<std::thread>(getMessages, std::ref(clientSocket));

  for (auto& thread : threads) {
    if (thread->joinable()) {
      thread->join();
    }
  }

  closesocket(clientSocket);
  system("pause");
  WSACleanup();
  return 0;
}