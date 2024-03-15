//#include <iostream>
//#include <Windows.h>
//#include <process.h>
//#include <winsock2.h>
//#include <ws2bth.h>
//#include <BluetoothAPIs.h>
//
//#pragma comment(lib, "ws2_32.lib")
//
//#define BUFFER_SIZE 1024
//#define DEBUGG true
//
//
//SOCKET createBluetoothSocket() {
//    SOCKET serverSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
//    if (serverSocket == INVALID_SOCKET) {
//        fprintf(stderr, "socket() failed with error: %ld\n", WSAGetLastError());
//    }
//    return serverSocket;
//}
//
//SOCKET bindBluetoothSocket(SOCKET serverSocket, SOCKADDR_BTH& serverAddr) {
//    memset(&serverAddr, 0, sizeof(SOCKADDR_BTH));
//    serverAddr.addressFamily = AF_BTH;
//    serverAddr.port = BT_PORT_ANY;  // Let the OS choose a port
//    serverAddr.serviceClassId = GUID_NULL;  // Use the default service class ID
//
//    int result = bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_BTH));
//    if (result == SOCKET_ERROR) {
//        fprintf(stderr, "bind() failed with error: %ld\n", WSAGetLastError());
//        closesocket(serverSocket);
//        serverSocket = INVALID_SOCKET;
//    }
//
//    return serverSocket;
//}
//
//SOCKET acceptBluetoothConnection(SOCKET serverSocket, SOCKADDR_BTH& clientAddr) {
//    SOCKET clientSocket = INVALID_SOCKET;
//    int clientAddrLen = sizeof(SOCKADDR_BTH);
//
//    clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);
//    if (clientSocket == INVALID_SOCKET) {
//        fprintf(stderr, "accept() failed with error: %ld\n", WSAGetLastError());
//    }
//
//    return clientSocket;
//}
//
//void sendToPipe(const char* message, HANDLE hPipe) {
//    DWORD bytesWritten;
//    if (ConnectNamedPipe(hPipe, NULL) != FALSE) {
//        WriteFile(hPipe, message, strlen(message), &bytesWritten, NULL);
//        DisconnectNamedPipe(hPipe);
//    }
//}
//
//void bluetoothProcessingThread(SOCKET clientSocket, HANDLE hPipe) {
//    char buffer[BUFFER_SIZE];
//    int bytesRead;
//
//    while (1) {
//        bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
//        if (bytesRead <= 0) {
//            break;
//        }
//
//        buffer[bytesRead] = '\0';  // Null-terminate the received data
//        if (DEBUGG)
//            printf("Received message from Bluetooth: %s\n", buffer);
//
//        // Send the received message to the pipe
//        sendToPipe(buffer, hPipe);
//    }
//}