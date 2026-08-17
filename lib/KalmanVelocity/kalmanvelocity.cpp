#include "kalmanvelocity.h"

float KalmanVelocity::estimate = 0.0f;
float KalmanVelocity::errorEstimate = 10.0f;
float KalmanVelocity::q = 0.5f;

bool KalmanVelocity::debugEnabled = false;
HardwareSerial* KalmanVelocity::debugSerial = nullptr;

void KalmanVelocity::begin() {
    estimate = 0.0f; // speed unknown at boot, starts at 0
    errorEstimate = PreferencesHandle::getInstance().getKalmanInitialErrorEstimate();
    q = PreferencesHandle::getInstance().getKalmanProcessNoise();

    debugPrint("Kalman filter initialized. errorEstimate=" + String(errorEstimate) +
               " q=" + String(q) + " (loaded from Preferences)");
}

float KalmanVelocity::update(float measurement, float measurementUncertainty) {
    float kalmanGain = errorEstimate / (errorEstimate + measurementUncertainty);

    estimate = estimate + kalmanGain * (measurement - estimate);
    errorEstimate = (1.0f - kalmanGain) * errorEstimate + fabs(estimate) * q;

    debugPrint("measurement=" + String(measurement) +
               " uncertainty=" + String(measurementUncertainty) +
               " gain=" + String(kalmanGain, 4) +
               " -> estimate=" + String(estimate) +
               " errorEstimate=" + String(errorEstimate));

    return estimate;
}

float KalmanVelocity::getEstimate() {
    return estimate;
}

float KalmanVelocity::getErrorEstimate() {
    return errorEstimate;
}

void KalmanVelocity::reset() {
    estimate = 0.0f;
    errorEstimate = PreferencesHandle::getInstance().getKalmanInitialErrorEstimate();
    debugPrint("Filter reset. estimate=0, errorEstimate=" + String(errorEstimate));
}

void KalmanVelocity::setProcessNoise(float value) {
    q = value;
    PreferencesHandle::getInstance().setKalmanProcessNoise(value);
    debugPrint("Process noise (q) updated and saved: " + String(value));
}

void KalmanVelocity::setInitialErrorEstimate(float value) {
    PreferencesHandle::getInstance().setKalmanInitialErrorEstimate(value);
    debugPrint("Initial error estimate updated and saved: " + String(value));
}

void KalmanVelocity::setDebugSerial(HardwareSerial* serial) {
    debugSerial = serial;
}

void KalmanVelocity::enableDebug(bool enable) {
    debugEnabled = enable;
}

void KalmanVelocity::debugPrint(String message) {
    if (debugEnabled && debugSerial != nullptr) {
        debugSerial->println("[KalmanVelocity] " + message);
    }
}