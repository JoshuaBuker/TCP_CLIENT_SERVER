#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <thread>

#pragma comment(lib,"Ws2_32.lib")
using std::cout, std::string;

const int port = 55555;

int sendMessages(SOCKET& clientSocket) {
    char buffer[200];

    while (true) {
        std::cin.clear();
        fflush(stdin);
        cout << "Enter your message to the server: ";
        std::cin.getline(buffer, 200);
        if (buffer == 0) {
            //return 0;
        }

        int byteCount = send(clientSocket, buffer, 200, 0);

        if (~byteCount > 0) {
            cout << "Message has failed to send!\n";
            //return -1;
        }
        else {
            std::cout << "Your Message: " << buffer << "\n"; // This just makes the clients message pop up on it's own screen as well
        }
    }

    return 0;
}

int getMessages(SOCKET& clientSocket) {
    char buffer[200];

    while (true) {
        int byteCount = recv(clientSocket, buffer, 200, 0);

        if (~byteCount > 0) {
            cout << "Message has failed to send!\n";
            //return -1;
        }
        else {
            cout << "\nOther User: " << buffer << "\n";
        }
        sendMessages(clientSocket);
    }

    return 0;
}

int Init() {
    // -------------------------------------------------------------


    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);

    // -------------------------------------------------------------

    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    if (wsaerr != 0) {
        cout << "The winsock dll not found!" << std::endl;
        return 1;
    }

    cout << "The Winsock dll found!" << "\n";
    cout << "The Status: " << wsaData.szSystemStatus << std::endl;

    return 0;

}

int ServerConnect(SOCKET clientSocket) {
    // -------------------------------------------------------------

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;

    InetPton(AF_INET, _T("192.168.56.1"), &clientService.sin_addr.s_addr);

    clientService.sin_port = htons(port);

    if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        cout << "Client: connect() - Failed To Connect: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    return 0;
}

int MakeThread(SOCKET clientSocket) {
    // -------------------------------------------------------------

    std::unique_ptr<std::thread> sendThread = std::make_unique<std::thread>(sendMessages, std::ref(clientSocket));
    std::unique_ptr<std::thread> receiveThread = std::make_unique<std::thread>(getMessages, std::ref(clientSocket));

    if (receiveThread && receiveThread->joinable()) {
        receiveThread->join();
    }
    if (sendThread && sendThread->joinable()) {
        sendThread->join();
    }

    return 0;
}

int main(void) {
    // -------------------------------------------------------------
    int ServerConn = Init();

    if (ServerConn == 1) { // If the socket fails, then it exits the program, just as the original code intended
        return 1;
    }

    SOCKET clientSocket;
    clientSocket = INVALID_SOCKET;

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET) {
        cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    }

    cout << "socket() is OK!" << std::endl;

    int Status = ServerConnect(clientSocket);
    if (Status == -1) {
        return 0;
    }
    cout << "Client: connect() is OK!\n" << "Client: Can start sending and receiving data..." << std::endl;

    MakeThread(clientSocket);
    // -------------------------------------------------------------

    closesocket(clientSocket);
    system("pause");
    WSACleanup();
    return 0;
}