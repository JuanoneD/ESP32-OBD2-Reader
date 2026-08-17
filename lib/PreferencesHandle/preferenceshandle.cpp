#include "preferenceshandle.h"

PreferencesHandle *PreferencesHandle::instance;

PreferencesHandle& PreferencesHandle::getInstance() {
    if(!instance){
        instance = new PreferencesHandle();
        instance->setVelocity(0);
        instance->setRPM(0);
        instance->setTemperature(0);
    }
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
    distanceTraveled = prefs.getFloat("distance", 0.0);
    tripFuelUsed = prefs.getFloat("tripFuel", 0.0);

    kalmanInitialErrorEstimate = prefs.getFloat("kalmanErrEst", 10.0);
    kalmanProcessNoise = prefs.getFloat("kalmanQ", 0.5);

    prefs.end();
}

void PreferencesHandle::savePreferences() {
    prefs.begin(PREFERENCE_NAMESPACE, false);
    prefs.putFloat("fuel", fuel);
    prefs.putFloat("capacity", tankCapacity);
    prefs.putFloat("factor", consumptionFactor);
    prefs.putFloat("distance", distanceTraveled);
    prefs.putFloat("tripFuel", tripFuelUsed);

    prefs.putFloat("kalmanErrEst", kalmanInitialErrorEstimate);
    prefs.putFloat("kalmanQ", kalmanProcessNoise);

    prefs.end();
}

void PreferencesHandle::setDistanceTraveled(float distance) {
    if(distance == this->distanceTraveled) return;
    this->distanceTraveled = distance;
    savePreferences();
}

float PreferencesHandle::getDistanceTraveled() {
    return distanceTraveled;
}

float PreferencesHandle::getTripFuelUsed() {
    return tripFuelUsed;
}

void PreferencesHandle::setTripFuelUsed(float fuel) {
    if(fuel == this->tripFuelUsed) return;
    this->tripFuelUsed = fuel;
    savePreferences();
}

void PreferencesHandle::setVelocity(float velocity) {
    this->velocity = velocity;
}

float PreferencesHandle::getVelocity() {
    return velocity;
}

void PreferencesHandle::setRPM(int rpm) {
    this->RPM = rpm;
}

int PreferencesHandle::getRPM() {
    return RPM;
}

void PreferencesHandle::setTemperature(int temperature) {
    this->temperature = temperature;
}

int PreferencesHandle::getTemperature() {
    return temperature;
}

float PreferencesHandle::getKalmanInitialErrorEstimate() {
    return kalmanInitialErrorEstimate;
}

void PreferencesHandle::setKalmanInitialErrorEstimate(float value) {
    if (value == this->kalmanInitialErrorEstimate) return;
    this->kalmanInitialErrorEstimate = value;
    savePreferences();
}

float PreferencesHandle::getKalmanProcessNoise() {
    return kalmanProcessNoise;
}

void PreferencesHandle::setKalmanProcessNoise(float value) {
    if (value == this->kalmanProcessNoise) return;
    this->kalmanProcessNoise = value;
    savePreferences();
}