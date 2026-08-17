#include "lcdhandler.h"

LiquidCrystal_I2C *LCDHandler::lcd = nullptr;
std::queue<String> LCDHandler::messageQueue;
unsigned long LCDHandler::lastMessageTime = 0;
int LCDHandler::lastRPMValue = 0;
int LCDHandler::lastVelocityValue = 0;
int LCDHandler::lastTemperatureValue = 0;
int LCDHandler::lastFuelLevelValue = 0;

void LCDHandler::begin() {
    lcd = new LiquidCrystal_I2C(0x27, 16, 2);
    lcd->init();
    delay(1000);
    lcd->backlight();

}

void LCDHandler::displayedVelocity() {
    if (lcd != nullptr) {
        int velocity = int(PreferencesHandle::getInstance().getVelocity());
        if(velocity == lastVelocityValue) return;

        lastVelocityValue = velocity;
        lcd->setCursor(0, 1);
        if(velocity < 100) lcd->print(" ");
        if(velocity < 10) lcd->print(" ");
        lcd->print(int(velocity));
        lcd->print("km/h");
    }
}

void LCDHandler::displayedRPM() {
    if (lcd != nullptr) {
        int rpm = PreferencesHandle::getInstance().getRPM();
        if(rpm == lastRPMValue) return;
        lastRPMValue = rpm;

        lcd->setCursor(0, 0);
        lcd->print("RPM: ");
        if(rpm < 1000) lcd->print(" ");
        lcd->print(rpm);
    }
}

void LCDHandler::displayedTemperature() {
    if (lcd != nullptr) {
        int temperature = PreferencesHandle::getInstance().getTemperature();
        if(temperature == lastTemperatureValue) return;
        lastTemperatureValue = temperature;

        lcd->setCursor(11, 1);
        if(temperature < 100) lcd->print(" ");
        if(temperature < 10) lcd->print(" ");
        lcd->print(temperature);
        lcd->write(223); // Caractere de grau (°)
        lcd->print("C");
    }
}

void LCDHandler::displayedFuelLevel() {
    if (lcd != nullptr) {
        int fuelLevel = int(PreferencesHandle::getInstance().getFuel() * 100 / PreferencesHandle::getInstance().getTankCapacity());
        if(fuelLevel == lastFuelLevelValue) return;
        lastFuelLevelValue = fuelLevel;

        lcd->setCursor(13, 0);
        lcd->print(fuelLevel);
        lcd->print("%");
    }
}

void LCDHandler::displayedStartedMessage() {
    if (lcd != nullptr) {
        lcd->setCursor(0, 0);
        lcd->print(" System Started");
    }
}

void LCDHandler::displayedTryingToConnectOBD() {
    if (lcd != nullptr) {
        lcd->setCursor(0, 0);
        lcd->print("Connecting OBD...");
    }
}

void LCDHandler::clearDisplay() {
    if (lcd != nullptr) {
        lcd->clear();
    }
}

void LCDHandler::displayedWaitECU() {
    if (lcd != nullptr) {
        lcd->setCursor(0, 0);
        lcd->print("Connect with OBD");
        lcd->setCursor(0, 1);
        lcd->print("  Wait ECU...   ");
    }
}

void LCDHandler::displayedECUAwake() {
    if (lcd != nullptr) {
        lcd->setCursor(0, 1);
        lcd->print("   ECU Awake!   ");
    }
}

void LCDHandler::update() {
    if(lcd == nullptr) return;
    unsigned long currentMillis = millis();
    float deltaTime = (currentMillis - lastMessageTime);

    if(deltaTime < UPDATED_LCD_INTERVAL_MS) return;
    
    displayedRPM();
    displayedFuelLevel();
    displayedVelocity();
    displayedTemperature();
    lastMessageTime = currentMillis;
    
}