#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "../lib/datadefinition.h"
#include <obdhandle.h>
#include <messagehandle.h>
#include <htmlinterface.h>
#include <preferenceshandle.h>
#include <lcdhandler.h>
#include <accelerometerhandle.h>
#include <gpshandle.h>
#include <kalmanvelocity.h>

// sensor address
static String targetAddress = "66:1e:32:7a:35:0e";

// UUIDs
static String serviceUUID = "0000fff0-0000-1000-8000-00805f9b34fb";
static String charUUID_TX = "0000fff2-0000-1000-8000-00805f9b34fb"; // Write
static String charUUID_RX = "0000fff1-0000-1000-8000-00805f9b34fb"; // Read


HTMLInterface htmlInterface;


// cycle count
unsigned long previousMillisforMessages = 0;
// unsigned long previousMillisforMediumDelayMessages = 0; // OBD speed (010D) disabled - GPS/Accel handle velocity now
unsigned long previousMillisforLongDelayMessages = 0;
unsigned long previousMillisforAccelerometerUpdates = 0;

int messagesCount = 0;

// Var
CONNECTION_STATUS status = CONNECTION_STATUS::DISCONNECTED;
ECU_STATUS ecu_state = ECU_STATUS::SLEEP;

void TaskWiFi(void * pvParameters) {
  Serial.print("Run Wifi on Core: ");
    Serial.println(xPortGetCoreID());
    
    htmlInterface.begin(); 

    for(;;) {
        htmlInterface.handleClient();
        vTaskDelay(10 / portTICK_PERIOD_MS); // Pequena pausa para o watchdog
    }
}

void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); // LCD I2C bus (pins 21, 22)
    Wire.setClock(100000); // 100kHz I2C
    LCDHandler::begin();
    LCDHandler::displayedStartedMessage();

    KalmanVelocity::setDebugSerial(&Serial);
    KalmanVelocity::enableDebug(true);
    KalmanVelocity::begin();

    //AccelerometerHandle::setDebugSerial(&Serial);
    AccelerometerHandle::begin();
    GPSHandle::setDebugSerial(&Serial);
    GPSHandle::enableDebug(true);
    GPSHandle::begin();

    xTaskCreatePinnedToCore(
        TaskWiFi,      
        "WiFi_Task",   
        10000,         
        NULL,          
        1,             
        NULL,          
        0              
    );
 
    // Enable debug for OBDHandle
    //OBDHandle::setDebugSerial(&Serial);
    OBDHandle::enableDebug(true);

    OBDHandle::setServiceUUID(serviceUUID.c_str());
    OBDHandle::setCharUUID_TX(charUUID_TX.c_str());
    OBDHandle::setCharUUID_RX(charUUID_RX.c_str());
    OBDHandle::begin();
    
    //MessageHandle::enableDebug(true);
    MessageHandle::setDebugSerial(&Serial);

    MessageHandle::setECUState(&ecu_state);
}

void loop() {
  OBDHandle::update();
  GPSHandle::update();

  if(status == CONNECTION_STATUS::DISCONNECTED)
  {
    Serial.println("Trying to connect with OBD...");
    LCDHandler::displayedTryingToConnectOBD();

    if(OBDHandle::connect(targetAddress.c_str()))
    {
      status = CONNECTION_STATUS::CONNECTED;
      ecu_state = ECU_STATUS::SLEEP;
    }
    
    LCDHandler::clearDisplay();
    return; // Skip the rest of the loop until connected
  }

  if(ecu_state == ECU_STATUS::SLEEP)
  {
    LCDHandler::clearDisplay();
    Serial.println("Connect with OBD, Wait ECU.");
    LCDHandler::displayedWaitECU();
    while(ecu_state == ECU_STATUS::SLEEP) {
      OBDHandle::checkECU();
      delay(1000);
    }
    LCDHandler::displayedECUAwake();
    delay(1000);
    LCDHandler::clearDisplay();
  }

  unsigned long currentMillis = millis();
  float deltaTimeForMessages = (currentMillis - previousMillisforMessages);
  // float deltaTimeForMediumDelayMessages = (currentMillis - previousMillisforMediumDelayMessages); // OBD speed disabled
  float deltaTimeForLongDelayMessages = (currentMillis - previousMillisforLongDelayMessages);
  float deltaTimeForAccelerometerUpdates = (currentMillis - previousMillisforAccelerometerUpdates);

  if(deltaTimeForMessages > DEFAULT_MESSAGE_INTERVAL_MS) {
    messagesCount++;
    if(messagesCount == 1) {
      OBDHandle::addCommandToQueue("010C"); // RPM
    } else if(messagesCount == 2) {
      OBDHandle::addCommandToQueue("0107"); // Long Term Fuel Trim
    } else {
      OBDHandle::addCommandToQueue("0104"); // Engine Load
      messagesCount = 0; // Reset the message count after sending Engine Load
    }
    previousMillisforMessages = currentMillis;
  }
  // else if(deltaTimeForMediumDelayMessages > MEDIUM_DELAY_MESSAGE_INTERVAL_MS) {
  //   OBDHandle::addCommandToQueue("010D"); // Speed - disabled, GPS/Accel handle velocity via Kalman
  //   previousMillisforMediumDelayMessages = currentMillis;
  // }
  else if(deltaTimeForLongDelayMessages > LONG_DELAY_MESSAGE_INTERVAL_MS) {
    OBDHandle::addCommandToQueue("0105"); // Temperature
    previousMillisforLongDelayMessages = currentMillis;
  }
  else if(deltaTimeForAccelerometerUpdates > ACCELEROMETER_UPDATE_INTERVAL_MS) {
    AccelerometerHandle::updateCurrentVelocity();
    previousMillisforAccelerometerUpdates = currentMillis;
  }

  LCDHandler::update();
}