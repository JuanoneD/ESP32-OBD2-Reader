#ifndef KALMAN_VELOCITY_H
#define KALMAN_VELOCITY_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <preferenceshandle.h>

class KalmanVelocity {
public:
    static void begin(); // loads initialErrorEstimate and q from PreferencesHandle

    static float update(float measurement, float measurementUncertainty);

    static float getEstimate();
    static float getErrorEstimate();
    static void reset();

    // Adjust tuning at runtime (persists to PreferencesHandle)
    static void setProcessNoise(float value);
    static void setInitialErrorEstimate(float value);

    static void setDebugSerial(HardwareSerial* serial);
    static void enableDebug(bool enable);

private:
    static float estimate;
    static float errorEstimate;
    static float q;

    static bool debugEnabled;
    static HardwareSerial* debugSerial;

    static void debugPrint(String message);
};

#endif // KALMAN_VELOCITY_H