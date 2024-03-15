#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

void quaternion_multiply(double q1w, double q1x, double q1y, double q1z,
    double q2w, double q2x, double q2y, double q2z,
    double* rw, double* rx, double* ry, double* rz) {
    *rw = q1w * q2w - q1x * q2x - q1y * q2y - q1z * q2z;
    *rx = q1w * q2x + q1x * q2w + q1y * q2z - q1z * q2y;
    *ry = q1w * q2y - q1x * q2z + q1y * q2w + q1z * q2x;
    *rz = q1w * q2z + q1x * q2y - q1y * q2x + q1z * q2w;
}

void getIMUData(double* ax, double* ay, double* az,
    double* mx, double* my, double* mz,
    double* qw, double* qx, double* qy, double* qz) {
    *ax = 0.1; *ay = 0.0; *az = -9.8; 
    *mx = 1.0; *my = 0.0; *mz = 0.0; 
    *qw = 1.0; *qx = 0.0; *qy = 0.0; *qz = 0.0; 
}

void adjustOrientationWithMagnetometer(double* orientation,
    double mx, double my, double mz) {

    double heading = atan2(my, mx); 
    double adjustment = heading - orientation[2]; 

    orientation[2] += adjustment;  // yaw
}

void updatePositionUsingQuaternion(double* position, double* orientation,
    double ax, double ay, double az,
    double deltaTime) {

    double rw, rx, ry, rz;
    quaternion_multiply(orientation[0], orientation[1], orientation[2], orientation[3],
        0.0, ax, ay, az,
        &rw, &rx, &ry, &rz);
    quaternion_multiply(rx, ry, rz, rw,
        orientation[0], -orientation[1], -orientation[2], -orientation[3],
        &rw, &rx, &ry, &rz);

    position[0] += 0.5 * rx * deltaTime * deltaTime;
    position[1] += 0.5 * ry * deltaTime * deltaTime;
    position[2] += 0.5 * rz * deltaTime * deltaTime;
}

void updatePositionWithoutQuaternion(double* position,
    double ax, double ay, double az,
    double deltaTime) {
    position[0] += 0.5 * ax * deltaTime * deltaTime;
    position[1] += 0.5 * ay * deltaTime * deltaTime;
    position[2] += 0.5 * az * deltaTime * deltaTime;
}

int main2() {
    double positionWithOrientation[3] = { 0.0, 0.0, 0.0 };
    double positionWithoutOrientation[3] = { 0.0, 0.0, 0.0 };
    double orientation[4] = { 1.0, 0.0, 0.0, 0.0 };
    double ax, ay, az, mx, my, mz, qw, qx, qy, qz;
    double deltaTime = 1.0; 

    for (int i = 0; i < 10; ++i) {
        getIMUData(&ax, &ay, &az, &mx, &my, &mz, &qw, &qx, &qy, &qz);

        adjustOrientationWithMagnetometer(orientation, mx, my, mz);

        updatePositionUsingQuaternion(positionWithOrientation, orientation, ax, ay, az, deltaTime);
        updatePositionWithoutQuaternion(positionWithoutOrientation, ax, ay, az, deltaTime);

        printf("With Quaternion Orientation - Position: [%f, %f, %f]\n",
            positionWithOrientation[0], positionWithOrientation[1], positionWithOrientation[2]);
        printf("Without Quaternion Orientation - Position: [%f, %f, %f]\n",
            positionWithoutOrientation[0], positionWithoutOrientation[2], positionWithoutOrientation[1]);

        Sleep(1000);
    }

    return 0;
}



int main() {
    FILE* fp;
    fopen_s(&fp,"data.csv", "w+"); // Open the file for writing

    if (fp == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    // Write some data to the CSV file
    fprintf(fp, "Year, Sales\n");
    fprintf(fp, "2018, 400\n");
    fprintf(fp, "2019, 200\n");
    fprintf(fp, "2020, 300\n");
    fprintf(fp, "2021, 400\n");

    fclose(fp); // Close the file
    printf("CSV file created.\n");
    int result = system("python CSVToExcel.py");
    return 0;
}



