#include "obdhandle.h"

// Definição das variáveis estáticas
BLEUUID OBDHandle::serviceUUID;
BLEUUID OBDHandle::charUUID_TX;
BLEUUID OBDHandle::charUUID_RX;
BLERemoteCharacteristic* OBDHandle::pCharTX = nullptr;
BLERemoteCharacteristic* OBDHandle::pCharRX = nullptr;

BLEClient* OBDHandle::pClient = nullptr;
String OBDHandle::lastResponse = "";
bool OBDHandle::debugEnabled = false;
HardwareSerial* OBDHandle::debugSerial = nullptr;

void OBDHandle::setServiceUUID(const char* uuid) {
    serviceUUID = BLEUUID(uuid);
}

void OBDHandle::setCharUUID_TX(const char* uuid) {
    charUUID_TX = BLEUUID(uuid);
}

void OBDHandle::setCharUUID_RX(const char* uuid) {
    charUUID_RX = BLEUUID(uuid);
}

void OBDHandle::enableDebug(bool enable) {
    debugEnabled = enable;
}

void OBDHandle::setDebugSerial(HardwareSerial* serial) {
    debugSerial = serial;
}

void OBDHandle::debugPrint(String message) {
    if (debugEnabled && debugSerial != nullptr) {
        debugSerial->println("[OBDHandle] " + message);
    }
}

bool OBDHandle::begin() {
    BLEDevice::init("ESP32_PAINEL");
    pClient = BLEDevice::createClient();
    return  true;
}
void OBDHandle::notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    for (int i = 0; i < length; i++) {
        lastResponse += (char)pData[i];
    }
}

String OBDHandle::sendCommand(String command) {
    if(!command.endsWith("\r")) command += "\r";
    
    debugPrint("Sending: " + command);

    lastResponse = ""; 
    pCharTX->writeValue((uint8_t*)command.c_str(), command.length(), false);

    unsigned long startWait = millis();
    while(lastResponse == "" && (millis() - startWait < 800)) { 
      if (lastResponse.indexOf('>') != -1) {
        break;
      }
      delay(1);
    }
    
    debugPrint("Response time: " + String(millis() - startWait) + "ms");
    
    String cleanRes = lastResponse;
    lastResponse = "";
    cleanRes.replace(">", "");
    cleanRes.trim();
    
    debugPrint("Received: " + cleanRes);
    
    return cleanRes;
}

void OBDHandle::sendStarterCommand(){
    OBDHandle::sendCommand("ATZ");    // Reset the chip
    delay(1000);
    OBDHandle::sendCommand("ATE0");   // Echo Off
    OBDHandle::sendCommand("ATH0");   // Headers Off
    OBDHandle::sendCommand("ATSP1");  // FORD STREET Protocol
}

bool OBDHandle::connect(const char* address) {
    debugPrint("Attempting to connect to: " + String(address));
    
    BLEAddress targetAddress(address);

    if (pClient->connect(targetAddress)) {
        debugPrint("BLE connection successful");
        
        BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
        if (pRemoteService != nullptr) {
            debugPrint("Service found");
            
            pCharTX = pRemoteService->getCharacteristic(charUUID_TX);
            pCharRX = pRemoteService->getCharacteristic(charUUID_RX);
            
            if (pCharTX != nullptr && pCharRX != nullptr) {
                debugPrint("Characteristics found");
                
                if(pCharRX->canNotify()) {
                    pCharRX->registerForNotify([](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
                        OBDHandle::notifyCallback(pBLERemoteCharacteristic, pData, length, isNotify);
                    });
                }
                sendStarterCommand();
                debugPrint("Connection established successfully");
                return true;
            } else {
                debugPrint("ERROR: Characteristics not found");
            }
        } else {
            debugPrint("ERROR: Service not found");
        }
    } else {
        debugPrint("ERROR: BLE connection failed");
    }
    return false;
}

bool OBDHandle::checkECU() {
    debugPrint("Checking ECU status...");
    String response = sendCommand("0100\r");
    if (response.indexOf("41 00") != -1) {
        debugPrint("ECU is ON");
        return true; // ECU ON
    }
    debugPrint("ECU is OFF or not responding");
    delay(1000);
    return false; // ECU OFF
}