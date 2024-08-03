#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <winsock2.h>
#include <mutex>
#include <tchar.h>
#include <ws2tcpip.h>

#define MAX_CLIENTS 10
#define MESSAGE_BUFFER_SIZE 1024

using std::cout;

std::mutex clientMutex;

void getMessages(SOCKET client, std::vector<SOCKET>& clientSockets) {
    char buffer[MESSAGE_BUFFER_SIZE];

    while (true) {
        int byteCount = recv(client, buffer, MESSAGE_BUFFER_SIZE, 0);

        if (byteCount <= 0) {
            if (byteCount == 0) {
                std::cout << "Connection closed by client" << std::endl;
            } else {
                std::cout << "recv failed: " << WSAGetLastError() << std::endl;
            }
            closesocket(client);
            break;
        }

        buffer[byteCount] = '\0';
        printf("Client: %s\n", buffer);

        std::lock_guard<std::mutex> lock(clientMutex);
        for (SOCKET otherClient : clientSockets) {
            if (otherClient != client) {
                send(otherClient, buffer, byteCount, 0);
            }
        }
    }
}

SOCKET makeClientConnection(const SOCKET serverSocket) {
    SOCKET acceptSocket = accept(serverSocket, NULL, NULL);

    if (acceptSocket == INVALID_SOCKET) {
        std::cout << "Accept failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    }

    return acceptSocket;
}

void connectionManager(const SOCKET server, std::vector<std::unique_ptr<std::thread>>& clientThreads, std::vector<SOCKET>& clientSockets) {
    SOCKET clientSocket = makeClientConnection(server);
    clientSockets.push_back(clientSocket);

    for (int i = 0; i < MAX_CLIENTS; i++) {
      std::lock_guard<std::mutex> lock(clientMutex);
        if (clientThreads[i] == nullptr || !clientThreads[i]->joinable()) {
            
            if (clientSocket != INVALID_SOCKET) {
                clientThreads[i] = std::make_unique<std::thread>(getMessages, clientSocket, std::ref(clientSockets));
            }
            break;
        }
    }
}

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

int main(void) {
    std::vector<std::unique_ptr<std::thread>> clientThreads(MAX_CLIENTS);
    std::vector<SOCKET> clientSockets(MAX_CLIENTS);

    SOCKET server = makeServer();
    if (server == INVALID_SOCKET) {
        return 1;
    }

    while (true) {
        connectionManager(server, clientThreads, clientSockets);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    for (auto& clientThread : clientThreads) {
        if (clientThread && clientThread->joinable()) {
            clientThread->join();
        }
    }

    for (SOCKET clientSocket : clientSockets) {
        closesocket(clientSocket);
    }

    closesocket(server);
    WSACleanup();
    return 0;
}