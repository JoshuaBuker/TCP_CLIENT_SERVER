#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <vector>
#include <thread>

#define MESSAGE_BUFFER_SIZE 512
#define MAX_CLIENTS 16

using std::cout, std::string;

SOCKET makeServer() {
  const int port = 55555;
  int wsaerr;
  WORD wVersionRequested = MAKEWORD(2,2);
  SOCKET serverSocket;
  WSADATA wsaData;

  wsaerr = WSAStartup(wVersionRequested, &wsaData);

  if (wsaerr != 0) {
    cout << "The winsock dll not found!" << std::endl;
    return 0;
  }

  serverSocket = INVALID_SOCKET;
  serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (serverSocket == INVALID_SOCKET) {
    cout << "Error at socket(): " << WSAGetLastError() << std::endl;
    WSACleanup();
    return 0;
  }

  sockaddr_in service;
  service.sin_family = AF_INET;

  InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
  service.sin_port = htons(port);

  if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
    cout << "bind() failed: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return 0;
  }

  if (listen(serverSocket, 1) == SOCKET_ERROR) {
    cout << "listen(): Error listening on socket " << WSAGetLastError << std::endl;
    WSACleanup();
    return 0;
  }

  return serverSocket;
}

SOCKET makeClientConnection(const SOCKET serverSocket) {
  SOCKET acceptSocket = accept(serverSocket, NULL, NULL);

  if (acceptSocket == INVALID_SOCKET) {
    cout << "Accept failed: " << WSAGetLastError << std::endl;
    WSACleanup();
    return 0;
  }

  return acceptSocket;
}

int getMessages(const SOCKET client) {
  char buffer[MESSAGE_BUFFER_SIZE];

  while (true) {
    int byteCount = recv(client, buffer, MESSAGE_BUFFER_SIZE, 0);

    if (byteCount <= 0) {
      continue;
    }

    printf("Client: %s\n", buffer);
    
  }
}

int connectionManager(SOCKET server, std::vector<std::thread>& clients) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (!clients[i].joinable()) {
      clients[i] = std::thread(getMessages, makeClientConnection(server));
      clients[i].join();
      break;
    }
  }
  return 1;
}

int main(void) {
  std::vector<std::thread> clients(MAX_CLIENTS);

  SOCKET server = makeServer();

  while (true) {
    connectionManager(server, clients);
  }

  system("pause");
  WSACleanup();
  return 0;
}