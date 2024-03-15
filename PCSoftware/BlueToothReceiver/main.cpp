//// main.cpp
//#include <iostream>
//#include <Windows.h>
//#include <process.h>
//#include "BlueToothReceiver.h"
//#include "PositionPrediction.h"
//
//int main2() {
//    HANDLE pipeRead, pipeWrite;
//
//    // Create an anonymous pipe for bidirectional communication
//    SECURITY_ATTRIBUTES saAttr;
//    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
//    saAttr.bInheritHandle = TRUE;
//    saAttr.lpSecurityDescriptor = NULL;
//
//    if (!CreatePipe(&pipeRead, &pipeWrite, &saAttr, 0)) {
//        std::cerr << "Error creating anonymous pipe." << std::endl;
//        return 1;
//    }
//
//    // Start your Bluetooth thread (writeThread) here
//    HANDLE hPipe = pipeWrite; // Pass the handle to your Bluetooth thread
//    HANDLE hBluetoothThread = (HANDLE)_beginthread(bluetoothProcessingThread(), 0, (void*)hPipe);
//
//    // Start your main processing thread (readThread) here
//    HANDLE hPipeRead = pipeRead; // Pass the handle to your main processing thread
//    HANDLE hMainThread = (HANDLE)_beginthread(readThread, 0, (void*)hPipeRead);
//
//    // Wait for user input to exit
//    std::cout << "Press Enter to exit..." << std::endl;
//    std::cin.get();
//
//    // Clean up and close handles
//    CloseHandle(pipeRead);
//    CloseHandle(pipeWrite);
//    CloseHandle(hBluetoothThread);
//    CloseHandle(hMainThread);
//
//    return 0;
//}
