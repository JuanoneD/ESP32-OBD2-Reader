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
private:
    static PreferencesHandle *instance;
    PreferencesHandle();
    Preferences prefs;

    float fuel;
    float tankCapacity;
    float consumptionFactor;
    void savePreferences();
};

#endif
