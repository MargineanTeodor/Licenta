#pragma once
#pragma once

#include <WinSock2.h>
#include <WS2bth.h>
#include <BluetoothAPIs.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024
#define DEBUGG true

SOCKET createBluetoothSocket();
SOCKET bindBluetoothSocket(SOCKET serverSocket, SOCKADDR_BTH& serverAddr);
SOCKET acceptBluetoothConnection(SOCKET serverSocket, SOCKADDR_BTH& clientAddr);
void sendToPipe(const char* message, HANDLE hPipe);
void bluetoothProcessingThread(SOCKET clientSocket, HANDLE hPipe);
