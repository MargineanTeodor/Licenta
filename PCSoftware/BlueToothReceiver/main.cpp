#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include "PositionPrediction.h"
#include "BlueToothReceiver.h"

int numberOfSeconds = 10;

int main() {
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::string> messageQueue;

    std::thread positionPredictionThread(mainPosPrediction, std::ref(messageQueue), std::ref(cv), std::ref(mtx), numberOfSeconds);
    std::thread positionBTConnection(mainBTConnection, std::ref(messageQueue), std::ref(cv), std::ref(mtx));

    positionPredictionThread.join();
    positionBTConnection.join();

    return 0;
}
