#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>

using std::cout, std::string;

int getMessages(SOCKET& clientSocket) {
  char buffer[1024];

  while (true) {
    memset(buffer, 0, sizeof(buffer));
    int byteCount = recv(clientSocket, buffer, 1024, 0);
    cout << "\nOther User: " << buffer << "\n";
  }
}

int sendMessages(SOCKET& clientSocket) {
  char buffer[1024];

  while (true) {
    std::cin.clear();
    fflush(stdin);
    cout << "Enter your message to the server: ";
    std::cin.getline(buffer, 1024);

    int byteCount = send(clientSocket, buffer, 1024, 0);

    if (!(byteCount > 0)) {
      cout << "Message has failed to send!\n";
    }
  }
}

int main(void) {
  // -------------------------------------------------------------

  SOCKET clientSocket;
  const int port = 55555;
  WSADATA wsaData;
  int wsaerr;
  WORD wVersionRequested = MAKEWORD(2, 2);

  // -------------------------------------------------------------

  wsaerr = WSAStartup(wVersionRequested, &wsaData);

  if (wsaerr != 0) {
    cout << "The winsock dll not found!" << std::endl;
    return 0;
  }

  cout << "The Winsock dll found!" << "\n";
  cout << "The Status: " << wsaData.szSystemStatus << std::endl;

  // -------------------------------------------------------------

  clientSocket = INVALID_SOCKET;

  clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (clientSocket == INVALID_SOCKET) {
    cout << "Error at socket(): " << WSAGetLastError() << std::endl;
    WSACleanup();
    return 0;
  }

  cout << "socket() is OK!" << std::endl;

  // -------------------------------------------------------------

  sockaddr_in clientService;
  clientService.sin_family = AF_INET;

  InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);

  clientService.sin_port = htons(port);

  if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
    cout << "Client: connect() - Failed To Connect: " << WSAGetLastError() << std::endl;
    WSACleanup();
    return 0;
  }

  cout << "Client: connect() is OK!\n" << "Client: Can start sending and receiving data..." << std::endl;

  // -------------------------------------------------------------

  std::unique_ptr<std::thread> sendThread = std::make_unique<std::thread>(sendMessages, std::ref(clientSocket));
  std::unique_ptr<std::thread> receiveThread = std::make_unique<std::thread>(getMessages, std::ref(clientSocket));

  if (receiveThread && receiveThread->joinable()) {
      receiveThread->join();
  }
  if (sendThread && sendThread->joinable()) {
      sendThread->join();
  }
  // -------------------------------------------------------------

  closesocket(clientSocket);
  system("pause");
  WSACleanup();
  return 0;
}