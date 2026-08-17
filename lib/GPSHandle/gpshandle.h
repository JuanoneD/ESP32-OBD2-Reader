#ifndef GPS_HANDLE_H
#define GPS_HANDLE_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <preferenceshandle.h>
#include <kalmanvelocity.h>
#include "../datadefinition.h"

class GPSHandle {
public:
    static void begin();
    static void update(); // call this every loop() cycle, non-blocking

    static bool hasFix();
    static double getSpeedKmh();
    static double getLatitude();
    static double getLongitude();
    static int getSatellites();

    static void setDebugSerial(HardwareSerial* serial);
    static void enableDebug(bool enable);

private:
    static TinyGPSPlus gps;
    static HardwareSerial gpsSerial;

    static bool debugEnabled;
    static HardwareSerial* debugSerial;

    static unsigned long lastFixTime;
    static bool initialized;

    static void debugPrint(String message);
    static void setUpdateRate2Hz();
};

#endif // GPS_HANDLE_H