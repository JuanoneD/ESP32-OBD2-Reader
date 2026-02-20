#include "preferenceshandle.h"

PreferencesHandle *PreferencesHandle::instance;

PreferencesHandle& PreferencesHandle::getInstance() {
    if(!instance)
        instance = new PreferencesHandle();
    return *instance;
}

void PreferencesHandle::setFuel(float fuel) {
    if(fuel == this->fuel) return;
    this->fuel = fuel;
    savePreferences();
}

void PreferencesHandle::setTankCapacity(float capacity) {
    if(capacity == this->tankCapacity) return;
    this->tankCapacity = capacity;
    savePreferences();
}

void PreferencesHandle::setConsumptionFactor(float factor) {
    if(factor == this->consumptionFactor) return;
    this->consumptionFactor = factor;
    savePreferences();
}

float PreferencesHandle::getFuel() {
    return fuel;
}

float PreferencesHandle::getTankCapacity() {
    return tankCapacity;
}

float PreferencesHandle::getConsumptionFactor() {
    return consumptionFactor;
}

PreferencesHandle::PreferencesHandle() {
    prefs.begin(PREFERENCE_NAMESPACE, true);
    fuel = prefs.getFloat("fuel", 45.0);
    tankCapacity = prefs.getFloat("capacity", 45.0);
    consumptionFactor = prefs.getFloat("factor", 0.00000000815);
    prefs.end();
}

void PreferencesHandle::savePreferences() {
    prefs.begin(PREFERENCE_NAMESPACE, false);
    prefs.putFloat("fuel", fuel);
    prefs.putFloat("capacity", tankCapacity);
    prefs.putFloat("factor", consumptionFactor);
    prefs.end();
}
