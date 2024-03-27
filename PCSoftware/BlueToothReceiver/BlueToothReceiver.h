#ifndef BLUETOOTHRECEIVER_H
#define BLUETOOTHRECEIVER_H

#include <windows.h>
#include <stdio.h>
#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>

HANDLE TryOpenSerialPort(LPCWSTR portName, int maxRetries, int retryDelay);

int mainBTConnection(std::queue<std::string>& messageQueue, std::condition_variable& cv, std::mutex& mtx);

#endif // BLUETOOTHRECEIVER_H
