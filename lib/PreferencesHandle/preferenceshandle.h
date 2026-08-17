#ifndef PREFERENCES_HANDLE_H
#define PREFERENCES_HANDLE_H

#include <Preferences.h>
#include "../datadefinition.h"

class PreferencesHandle {
public:
    static PreferencesHandle& getInstance();
    float getFuel();
    void setFuel(float fuel);
    float getTankCapacity();
    void setTankCapacity(float capacity);
    float getConsumptionFactor();
    void setConsumptionFactor(float factor);
    float getDistanceTraveled();
    void setDistanceTraveled(float distance);
    float getTripFuelUsed();
    void setTripFuelUsed(float fuel);
    void setVelocity(float velocity);
    float getVelocity();
    void setRPM(int rpm);
    int getRPM();
    void setTemperature(int temperature);
    int getTemperature();

    // Kalman filter tuning (persisted so it survives reboot)
    float getKalmanInitialErrorEstimate();
    void setKalmanInitialErrorEstimate(float value);
    float getKalmanProcessNoise();
    void setKalmanProcessNoise(float value);

private:
    static PreferencesHandle *instance;
    PreferencesHandle();
    Preferences prefs;

    float fuel;
    float tankCapacity;
    float consumptionFactor;
    float distanceTraveled;
    float tripFuelUsed;
    float velocity;
    int RPM;
    int temperature;

    float kalmanInitialErrorEstimate;
    float kalmanProcessNoise;

    void savePreferences();
};

#endif