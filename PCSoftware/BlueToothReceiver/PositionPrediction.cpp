#include "PositionPrediction.h"

using namespace std;

void getIMUData(double* ax, double* ay, double* az, //// versiune dummy 
    double* mx, double* my, double* mz,
    double* qw, double* qx, double* qy, double* qz) {
    *ax = 0.1; *ay = 0.0; *az = -9.8; 
    *mx = 1.0; *my = 0.0; *mz = 0.0; 
    *qw = 1.0; *qx = 0.0; *qy = 0.0; *qz = 0.0; 
}

void getIMUDataMutex(double* ax, double* ay, double* az, //// versiune din care ia din messageQueue
    double* mx, double* my, double* mz,
    double* qw, double* qx, double* qy, double* qz, double* qwm, double* qxm, double* qym, double* qzm,
    std::queue<std::string>& messageQueue, std::condition_variable& cv, std::mutex& mtx) {

    float numbers[14];
    int i = 0;
    char* context = nullptr;

    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&messageQueue] { return !messageQueue.empty(); });
    string message = messageQueue.front();
    messageQueue.pop();
    lock.unlock();

    char* cstr = new char[message.length() + 1];
    strcpy_s(cstr, message.length() + 1, message.c_str());

    char* token = strtok_s(cstr, " ",&context);
    while (token != NULL && i < 14) {
        
        numbers[i] = atof(token);
        i++;
        token = strtok_s(NULL, " ", &context);
    }

    if (i != 12) {
        printf("Error: The input does not contain 14 numbers.\n"); // format normal accelrometru, magnetometru, qaternioniSiplu, quaternionModificat
    }
    else
    {
        *ax = numbers[0];
        *ay = numbers[1];
        *az = numbers[2];
        *mx = numbers[3];
        *my = numbers[4];
        *mz = numbers[5];
        *qw = numbers[6];
        *qx = numbers[7];
        *qy = numbers[8];
        *qz = numbers[9];
        *qwm = numbers[10];
        *qxm = numbers[11];
        *qym = numbers[12];
        *qzm = numbers[13];
    }
}

void updatePositionUsingQuaternion(double* position, double qw,double qx, double qy, double qz,
    double ax, double ay, double az,
    double deltaTime) {

    double axE = 2.0 * (qw * qw + qx * qx - 0.5) * ax + 2.0 * (qx * qy - qw * qz) * ay + 2.0 * (qx * qz + qw * qy) * az;
    double ayE = 2.0 * (qx * qz - qw * qy) * ax + 2.0 * (qy * qz + qw * qx) * ay + 2.0 * (qw * qw + qz * qz - 0.5) * az;
    double azE = 2.0 * (qx * qy + qw * qz) * ax + 2.0 * (qw * qw + qy * qy - 0.5) * ay + 2.0 * (qy * qz - qw * qx) * az;

    position[0] += 0.5 * axE * deltaTime * deltaTime;
    position[1] += 0.5 * ayE * deltaTime * deltaTime;
    position[2] += 0.5 * azE * deltaTime * deltaTime;
}

void adjustOrientationWithMagnetometer(double* orientation,
    double mx, double my, double mz) {

    double heading = atan2(my, mx); 
    double adjustment = heading - orientation[2]; 

    orientation[2] += adjustment;  // yaw
}

void updatePositionAndOrientation(double* position, double* orientation,
    double ax, double ay, double az,
    double mx, double my, double mz,
    double deltaTime) {

    adjustOrientationWithMagnetometer(orientation, mx, my, mz);

    double radYaw = orientation[2]; // Yaw
    double axAdjusted = ax * cos(radYaw) - ay * sin(radYaw);
    double ayAdjusted = ax * sin(radYaw) + ay * cos(radYaw);

    position[0] += 0.5 * axAdjusted * deltaTime * deltaTime;
    position[1] += 0.5 * ayAdjusted * deltaTime * deltaTime;
    position[2] += 0.5 * az * deltaTime * deltaTime; 
}

void makeGraphs(double *XQ, double *YQ, double *ZQ, double *XQM, double *YQM, double *ZQM, double* XNQ, double* YNQ, double* ZNQ, int nrSec) {
    FILE* fp;
    fopen_s(&fp, "data.csv", "w+");

    if (fp == NULL) {
        printf("Error opening file\n");
    }
    else {

        fprintf(fp, "XWithQuaternion, YWithQuaternion, ZWithQuaternion, XWithQuaternionMod, YWithQuaternionMod, ZWithQuaternionMod, XWithoutQuaternion, YWithoutQuaternion, ZWithoutQuaternion\n");

        for (int i = 0; i < nrSec; i++)
        {
            char strXQ[8], strYQ[8], strZQ[8], strXQM[8], strYQM[8], strZQM[8], strXNQ[8], strYNQ[8], strZNQ[8];
            snprintf(strXQ, sizeof(strXQ), "%f", XQ[i]);
            snprintf(strYQ, sizeof(strYQ), "%f", YQ[i]);
            snprintf(strZQ, sizeof(strZQ), "%f", ZQ[i]);
            snprintf(strXQM, sizeof(strXQM), "%f", XQM[i]);
            snprintf(strYQM, sizeof(strYQM), "%f", YQM[i]);
            snprintf(strZQM, sizeof(strZQM), "%f", ZQM[i]);
            snprintf(strXNQ, sizeof(strXNQ), "%f", XNQ[i]);
            snprintf(strYNQ, sizeof(strYNQ), "%f", YNQ[i]);
            snprintf(strZNQ, sizeof(strZNQ), "%f", ZNQ[i]);
            fprintf(fp, "%s, %s, %s, %s, %s, %s, %s, %s, %s\n", strXQ, strYQ, strZQ, strXQM, strYQM, strZQM, strXNQ, strYNQ, strZNQ);
        }
        fclose(fp);
        char commandString[25];
        snprintf(commandString, sizeof(commandString), "python CSVToExcel.py %d", nrSec);
        int result = system(commandString);
    }
}

void mainPosPrediction(std::queue<std::string>& messageQueue, std::condition_variable& cv, std::mutex& mtx ,int numberOfSeconds)
{ /// facuta testare si scos exact termen de comparatie pe real 

    double* positionXWithQuaternion = (double*)malloc((numberOfSeconds+1) * sizeof(double));
    double* positionYWithQuaternion = (double*)malloc((numberOfSeconds+1) * sizeof(double));
    double* positionZWithQuaternion = (double*)malloc((numberOfSeconds+1) * sizeof(double));
    double* positionXWithQuaternionModify = (double*)malloc((numberOfSeconds + 1) * sizeof(double));
    double* positionYWithQuaternionModify = (double*)malloc((numberOfSeconds + 1) * sizeof(double));
    double* positionZWithQuaternionModify = (double*)malloc((numberOfSeconds + 1) * sizeof(double));
    double* positionXWithoutQuaternion = (double*)malloc((numberOfSeconds + 1) * sizeof(double));
    double* positionYWithoutQuaternion = (double*)malloc((numberOfSeconds + 1) * sizeof(double));
    double* positionZWithoutQuaternion = (double*)malloc((numberOfSeconds + 1) * sizeof(double));

    double positionWithOrientation[3] = { 0.0, 0.0, 0.0 };
    double positionWithoutOrientation[3] = { 0.0, 0.0, 0.0 };
    double positionWithOrientationModify[3] = { 0.0, 0.0, 0.0 };

    double orientation[4] = { 1.0, 0.0, 0.0, 0.0 };
    double ax, ay, az, mx, my, mz, qw, qx, qy, qz, qwm, qxm, qym, qzm;
    double deltaTime = 1.0; 

    for (int i = 0; i < numberOfSeconds; ++i) { //// /de aranjat aici sa fie secunde fara sa opresti efectiv procesarea
       //getIMUDataMutex(&ax, &ay, &az, &mx, &my, &mz, &qw, &qx, &qy, &qz, &qwm, &qxm, &qym, &qzm, messageQueue, cv, mtx);
        getIMUData(&ax, &ay, &az, &mx, &my, &mz, &qw, &qx, &qy, &qz);

        adjustOrientationWithMagnetometer(orientation, mx, my, mz);

        updatePositionUsingQuaternion(positionWithOrientation, qw, qx, qy, qz, ax, ay, az, deltaTime);
        updatePositionUsingQuaternion(positionWithOrientationModify, qw, qx, qy, qz, ax, ay, az, deltaTime);
        updatePositionAndOrientation(positionWithoutOrientation, orientation, ax, ay, az, mx, my, mz, deltaTime);

        printf("With Quaternion Orientation - Position: [%f, %f, %f]\n",
            positionWithOrientation[0], positionWithOrientation[1], positionWithOrientation[2]);
        printf("Without Quaternion Orientation - Position: [%f, %f, %f]\n",
            positionWithoutOrientation[0], positionWithoutOrientation[2], positionWithoutOrientation[1]);
        positionXWithQuaternion[i] = positionWithOrientation[0];
        positionYWithQuaternion[i] = positionWithOrientation[1];
        positionZWithQuaternion[i] = positionWithOrientation[2];
        positionXWithQuaternionModify[i] = positionWithOrientationModify[0];
        positionYWithQuaternionModify[i] = positionWithOrientationModify[1];
        positionZWithQuaternionModify[i] = positionWithOrientationModify[2];
        positionXWithoutQuaternion[i] = positionWithoutOrientation[0];
        positionYWithoutQuaternion[i] = positionWithoutOrientation[1];
        positionZWithoutQuaternion[i] = positionWithoutOrientation[2];
        Sleep(1000);
    }

    makeGraphs(positionXWithQuaternion,positionYWithQuaternion,positionZWithQuaternion,
        positionXWithQuaternionModify, positionYWithQuaternionModify, positionZWithQuaternionModify, 
        positionXWithoutQuaternion, positionYWithoutQuaternion, positionZWithoutQuaternion, 
        numberOfSeconds);
   
    free(positionXWithQuaternion);
    free(positionYWithQuaternion);
    free(positionZWithQuaternion);
    free(positionXWithQuaternionModify);
    free(positionYWithQuaternionModify);
    free(positionZWithQuaternionModify);
    free(positionXWithoutQuaternion);
    free(positionYWithoutQuaternion);
    free(positionZWithoutQuaternion);
}
