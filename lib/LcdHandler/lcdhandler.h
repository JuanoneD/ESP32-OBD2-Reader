#ifndef LCD_HANDLER_H
#define LCD_HANDLER_H

#include <LiquidCrystal_I2C.h>
#include <preferenceshandle.h>
#include <queue>

class LCDHandler {
public:
    static void begin();
    static void displayedTryingToConnectOBD();
    static void displayedStartedMessage();
    static void displayedWaitECU();
    static void displayedECUAwake();
    static void clearDisplay();
    static void update();

    
private:
    static LiquidCrystal_I2C *lcd;
    static std::queue<String> messageQueue;
    static unsigned long lastMessageTime;
    static int lastRPMValue;
    static int lastVelocityValue;
    static int lastTemperatureValue;
    static int lastFuelLevelValue;

    static void displayedVelocity();
    static void displayedRPM();
    static void displayedTemperature();
    static void displayedFuelLevel();
};

#endif // LCD_HANDLER_H