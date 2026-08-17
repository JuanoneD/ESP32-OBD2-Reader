#include "messagehandle.h"
#include <ctime>
#include <kalmanvelocity.h>

bool MessageHandle::debugEnabled = false;
HardwareSerial* MessageHandle::debugSerial = nullptr;
ECU_STATUS* MessageHandle::ecu_state = nullptr;
unsigned long MessageHandle::lastEngineLoadRequestTime = 0;
float MessageHandle::lastLFTValue = 0;

void MessageHandle::processRPMMessage(String message) {
    int indexRPM = message.indexOf("410C");
    if (indexRPM != -1 && message.length() >= indexRPM + 8) {
        int A = strtol(message.substring(indexRPM + 4, indexRPM + 6).c_str(), NULL, 16);
        int B = strtol(message.substring(indexRPM + 6, indexRPM + 8).c_str(), NULL, 16);
        int rpm = ((A * 256) + B) / 4;
        PreferencesHandle::getInstance().setRPM(rpm);
    }
}

void MessageHandle::processTemperatureMessage(String message) {
    int index = message.indexOf("4105");
    
    if (index != -1 && message.length() >= index + 6) {
        String hexVal = message.substring(index + 4, index + 6);
        int tempDecimal = strtol(hexVal.c_str(), NULL, 16);
        int tempFinal = tempDecimal - 40;

        PreferencesHandle::getInstance().setTemperature(tempFinal);
        debugPrint(">>> Temp: " + String(tempFinal) + " C\n");
    }
}

void MessageHandle::processCheckECUMessage(String message) {
    if(ecu_state == nullptr) return;
    *ecu_state = ECU_STATUS::AWAKE;
    debugPrint("ECU is AWAKE.");
    lastEngineLoadRequestTime = 0;
}

void MessageHandle::processAndShowMessage(String message) {
    
    if(message.indexOf("NO DATA") != -1 || message.indexOf("ERROR") != -1) {
        debugPrint("ECU Connection is OFF.");
        if(ecu_state != nullptr) {
            *ecu_state = ECU_STATUS::SLEEP;

            // Feed the Kalman filter a confident "zero" instead of writing
            // straight to Preferences, so all sources stay consistent.
            float fusedSpeed = KalmanVelocity::update(0.0f, 0.5f);
            PreferencesHandle::getInstance().setVelocity(fusedSpeed);

            PreferencesHandle::getInstance().setTemperature(0);
            PreferencesHandle::getInstance().setRPM(0);
        }
        return;
    }
    
    int mux = strtol(message.substring(4, 6).c_str(), NULL, 16);

    debugPrint("Processing message with MUX: " + String(mux));

    String clearMessage = message;
    clearMessage.replace(" ", "");
    clearMessage.replace(">", "");
    clearMessage.trim();
    
    switch (mux) {
        case RPM_MUX:
            processRPMMessage(clearMessage);
            break;
        case TEMP_MUX:
            processTemperatureMessage(clearMessage);
            break;
        case CHECK_ECU_MUX:
            processCheckECUMessage(clearMessage);
            break;
        case ENGINE_LOAD_MUX:
            processEngineLoadMessage(clearMessage);
            break;
        case SPEED_MUX:
            processSpeedMessage(clearMessage);
            break;
        case LONG_TERM_FUEL_TRIM_MUX:
            processLongTermFuelTrimMessage(clearMessage);
            break;
        default:
        break;
    }
}

void MessageHandle::processSpeedMessage(String message) {
    int index = message.indexOf("410D");
    unsigned long currentTime = millis();

    if (index != -1 && message.length() >= index + 6) {
        int speedKmhRaw = strtol(message.substring(index + 4, index + 6).c_str(), NULL, 16);
        float speedKmh = speedKmhRaw * 1.07f;

        float fusedSpeed = KalmanVelocity::update(speedKmh, 0.5f); // OBD confiável, incerteza baixa
        PreferencesHandle::getInstance().setVelocity(fusedSpeed);

        debugPrint("OBD raw speed: " + String(speedKmh) + " km/h | Fused: " + String(fusedSpeed) + " km/h");
    }
}

void MessageHandle::processLongTermFuelTrimMessage(String message) {
    int index = message.indexOf("4107");
    if (index != -1 && message.length() >= index + 6) {
        String hexVal = message.substring(index + 4, index + 6);
        int trimDecimal = strtol(hexVal.c_str(), NULL, 16);
        
        float trimFinal = (trimDecimal * (100.0 / 128.0)) - 100.0;
        
        debugPrint(">>> Long Term Fuel Trim: " + String(trimFinal) + " %\n");
        lastLFTValue = trimFinal;
    }
}

void MessageHandle::processEngineLoadMessage(String message) {
    int index = message.indexOf("4104");
    unsigned long currentTime = millis();

    if(lastEngineLoadRequestTime == 0) {
        lastEngineLoadRequestTime = currentTime;
        return;
    }
    
    if (index != -1 && message.length() >= index + 6) {
        String hexVal = message.substring(index + 4, index + 6);
        int loadDecimal = strtol(hexVal.c_str(), NULL, 16);
        int loadFinal = (loadDecimal * 100) / 255;
        float deltaTime = (currentTime - lastEngineLoadRequestTime)/1000.0; 
        float lftModifier = 1 + (lastLFTValue / 100.0);
        int lastRPMValue = PreferencesHandle::getInstance().getRPM();

        float fuelConsumption = (lastRPMValue * loadFinal * PreferencesHandle::getInstance().getConsumptionFactor() * lftModifier) * deltaTime;
        PreferencesHandle::getInstance().setFuel(PreferencesHandle::getInstance().getFuel() - fuelConsumption);

        float tripfuelConsumed = PreferencesHandle::getInstance().getTripFuelUsed() + fuelConsumption;
        PreferencesHandle::getInstance().setTripFuelUsed(tripfuelConsumed);

        lastEngineLoadRequestTime = currentTime;
        debugPrint(">>> Engine Load: " + String(loadFinal) + " %\n");
    }
}

void MessageHandle::enableDebug(bool enable) {
    debugEnabled = enable;
}

void MessageHandle::setDebugSerial(HardwareSerial* serial) {
    debugSerial = serial;
}

void MessageHandle::debugPrint(String message) {
    if (debugEnabled && debugSerial != nullptr) {
        debugSerial->println("[MessageHandle] " + message);
    }
}

void MessageHandle::setECUState(ECU_STATUS* state) {
    ecu_state = state;
}