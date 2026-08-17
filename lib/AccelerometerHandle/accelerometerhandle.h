#ifndef ACCELEROMETER_HANDLE_H
#define ACCELEROMETER_HANDLE_H

#include <MPU6050_light.h>
#include "../datadefinition.h"
#include <preferenceshandle.h>
#include <lcdhandler.h>
#include <kalmanvelocity.h>

class AccelerometerHandle {
public:
    static void begin();
    static void updateCurrentVelocity();
    static void setDebugSerial(HardwareSerial* serial);

private:
    static MPU6050 mpu;
    static bool initialized;
    static bool debugEnabled;
    static HardwareSerial* debugSerial;
    static float filteredAccelX;

    static float currentPitch;
    static unsigned long lastUpdateTime;

    static void debugPrint(String message);
    static void setDLPF(uint8_t mode);
    static void applyVote();

    static int voteCount;
    static int positiveVotes;
    static int negativeVotes;
};

#endif // ACCELEROMETER_HANDLE_H