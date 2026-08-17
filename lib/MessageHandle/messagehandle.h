#ifndef MESSAGEHANDLE_H
#define MESSAGEHANDLE_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "../datadefinition.h"
#include <preferenceshandle.h>
#include <lcdhandler.h>

class MessageHandle {
private:
    static bool debugEnabled;
    static HardwareSerial* debugSerial;
    static ECU_STATUS *ecu_state;
    static unsigned long lastEngineLoadRequestTime;
    static float lastLFTValue;

    static void processRPMMessage(String message);
    static void processTemperatureMessage(String message);
    static void processCheckECUMessage(String message);
    static void processEngineLoadMessage(String message);
    static void processSpeedMessage(String message);
    static void processLongTermFuelTrimMessage(String message);
    static void debugPrint(String message);
    
public:
    static void setECUState(ECU_STATUS* state);
    static void processAndShowMessage(String message);
    static void enableDebug(bool enable);
    static void setDebugSerial(HardwareSerial* serial);
};

#endif