#ifndef QUATERNION_FUNCTIONS_H
#define QUATERNION_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

// Function to multiply two quaternions
void quaternion_multiply(double q1w, double q1x, double q1y, double q1z,
    double q2w, double q2x, double q2y, double q2z,
    double* rw, double* rx, double* ry, double* rz);

// Function to simulate getting data from an IMU
void getIMUData(double* ax, double* ay, double* az,
    double* mx, double* my, double* mz,
    double* qw, double* qx, double* qy, double* qz);

// Function to adjust orientation using magnetometer data
void adjustOrientationWithMagnetometer(double* orientation,
    double mx, double my, double mz);

// Function to update position using quaternion for orientation
void updatePositionUsingQuaternion(double* position, double* orientation,
    double ax, double ay, double az,
    double deltaTime);

// Function to update position without using quaternion for orientation
void updatePositionWithoutQuaternion(double* position,
    double ax, double ay, double az,
    double deltaTime);

#endif // QUATERNION_FUNCTIONS_H
