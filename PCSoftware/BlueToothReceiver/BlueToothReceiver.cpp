#include "BlueToothReceiver.h"

HANDLE TryOpenSerialPort(LPCWSTR portName, int maxRetries, int retryDelay) {
    HANDLE hSerial = INVALID_HANDLE_VALUE;
    for (int attempt = 0; attempt < maxRetries && hSerial == INVALID_HANDLE_VALUE; ++attempt) {
        hSerial = CreateFile(portName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hSerial == INVALID_HANDLE_VALUE) {
            printf("Attempt %d: Could not open serial port. Retrying in %d seconds...\n", attempt + 1, retryDelay / 1000);
            Sleep(retryDelay); 
        }
    }
    return hSerial;
}

int mainBTConnection(std::queue<std::string>& messageQueue, std::condition_variable& cv, std::mutex& mtx) {
    char* portName = (char*)malloc(25 * sizeof(char));
    if (portName != NULL) {
        strcpy_s(portName, 25, "\\\\.\\COM3");
    }
    LPCWSTR portNameLPCW = L"\\\\.\\COM3\0";
    HANDLE hSerial = TryOpenSerialPort(portNameLPCW, 10, 2000);

    if (hSerial == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error: Unable to establish connection to the serial port\n");
        return 1;
    }
    printf("Successfully connected to the serial port.\n");

 
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Error getting serial port state\n");
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        fprintf(stderr, "Error setting serial port state\n");
        CloseHandle(hSerial);
        return 1;
    }

    char buffer[1024];
    DWORD bytesRead;
    while (1) {
        BOOL status = ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
        if (!status) {
            fprintf(stderr, "Error reading from serial port\n");
            break;
        }

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0'; 
            printf("%s", buffer);
            std::lock_guard<std::mutex> lock(mtx);
            messageQueue.push(buffer);
            cv.notify_one();
        }
    }
    CloseHandle(hSerial);

    return 0;
}
