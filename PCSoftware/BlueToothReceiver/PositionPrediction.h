#ifndef POSITIONPREDICTION_H
#define POSITIONPREDICTION_H

#include <windows.h>
#include <condition_variable>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <cmath>


#ifdef __cplusplus
extern "C" {
#endif

    void quaternion_multiply(double q1w, double q1x, double q1y, double q1z,
        double q2w, double q2x, double q2y, double q2z,
        double* rw, double* rx, double* ry, double* rz);

    void getIMUData(double* ax, double* ay, double* az,
        double* mx, double* my, double* mz,
        double* qw, double* qx, double* qy, double* qz);

    void getIMUDataMutex(double* ax, double* ay, double* az,
        double* mx, double* my, double* mz,
        double* qw, double* qx, double* qy, double* qz,
        double* qwm, double* qxm, double* qym, double* qzm,
        std::queue<std::string>& messageQueue, std::condition_variable& cv, std::mutex& mtx);

    void adjustOrientationWithMagnetometer(double* orientation,
        double mx, double my, double mz);

    void updatePositionUsingQuaternion(double* position, double* orientation,
        double ax, double ay, double az,
        double deltaTime);

    void updatePositionWithoutQuaternion(double* position,
        double ax, double ay, double az,
        double deltaTime);

    void makeGraphs(double* XQ, double* YQ, double* ZQ, double* XQM, double* YQM, double* ZQM,
        double* XNQ, double* YNQ, double* ZNQ, int nrSec);

    void mainPosPrediction(std::queue<std::string>& messageQueue, std::condition_variable& cv, std::mutex& mtx, int numberOfSeconds);


#ifdef __cplusplus
}
#endif

#endif // POSITIONPREDICTION_H
