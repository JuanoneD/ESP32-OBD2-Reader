#include "obdhandle.h"

BLEUUID OBDHandle::serviceUUID;
BLEUUID OBDHandle::charUUID_TX;
BLEUUID OBDHandle::charUUID_RX;
BLERemoteCharacteristic* OBDHandle::pCharTX = nullptr;
BLERemoteCharacteristic* OBDHandle::pCharRX = nullptr;
BLEClient* OBDHandle::pClient = nullptr;

String OBDHandle::lastResponse = "";
bool OBDHandle::messageReceived = false;
std::queue<String> OBDHandle::commandQueue = std::queue<String>();

bool OBDHandle::debugEnabled = false;
HardwareSerial* OBDHandle::debugSerial = nullptr;

bool OBDHandle::pendingSend = false;
String OBDHandle::pendingCommand = "";
unsigned long OBDHandle::lastCommandSentTime = 0;
unsigned long OBDHandle::currentTimeout = OBDHandle::DEFAULT_TIMEOUT;

void OBDHandle::debugPrint(String message) {
    if (debugEnabled && debugSerial != nullptr) {
        debugSerial->println("[OBDHandle] " + message);
    }
}

bool OBDHandle::begin() {
    BLEDevice::init("ESP32_PAINEL");
    pClient = BLEDevice::createClient();
    messageReceived = true; // nenhum comando pendente no início
    return true;
}

// Roda na task do BLE. NUNCA chama sendCommand aqui dentro — só sinaliza.
void OBDHandle::notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    if (messageReceived == true) {
        debugPrint("Warning: Unexpected notification while idle, discarding");
        return;
    }

    for (int i = 0; i < length; i++) {
        lastResponse += (char)pData[i];
    }

    if (lastResponse.indexOf('>') != -1) {
        MessageHandle::processAndShowMessage(lastResponse);
        debugPrint("Message Received: " + lastResponse);
        messageReceived = true;
        lastResponse = "";

        if (!commandQueue.empty()) {
            pendingCommand = commandQueue.front();
            commandQueue.pop();
            pendingSend = true; // sinaliza pro loop() principal enviar
        }
    }
}

// Só é chamado pela task principal (via update() ou addCommandToQueue direto)
void OBDHandle::sendCommand(String command) {
    if (pClient == nullptr || !pClient->isConnected() || pCharTX == nullptr) {
        debugPrint("Cannot send command, BLE not ready: " + command);
        return;
    }

    if (!command.endsWith("\r")) command += "\r";
    debugPrint("Sending: " + command);

    lastResponse = "";
    messageReceived = false;
    lastCommandSentTime = millis();
    currentTimeout = command.startsWith("AT") ? AT_COMMAND_TIMEOUT : DEFAULT_TIMEOUT;

    pCharTX->writeValue((uint8_t*)command.c_str(), command.length(), false);
}

// Chamar isso a cada ciclo do loop() principal
void OBDHandle::update() {
    // 1. Envia comando pendente sinalizado pelo callback
    if (pendingSend) {
        pendingSend = false;
        sendCommand(pendingCommand);
        return; // evita mandar timeout no mesmo ciclo que acabou de enviar
    }

    // 2. Checa timeout de comando sem resposta
    if (messageReceived == false && (millis() - lastCommandSentTime > currentTimeout)) {
        debugPrint("Command timeout after " + String(currentTimeout) + "ms, moving to next command");
        messageReceived = true;
        lastResponse = "";

        if (!commandQueue.empty()) {
            String nextCommand = commandQueue.front();
            commandQueue.pop();
            sendCommand(nextCommand);
        }
    }
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

                if (pCharRX->canNotify()) {
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

void OBDHandle::sendStarterCommand() {
    sendCommand("ATZ");
    delay(500); // wait for ECU to reset
    sendCommand("ATE0");
    delay(500); // wait for echo off
    sendCommand("ATH0");
    delay(500); // wait for headers off
    sendCommand("ATSP1");
    delay(500); // wait for protocol set
    sendCommand("ATAT1");
    delay(500); // wait for adapter type set
    sendCommand("ATL0");
    delay(500); // wait for linefeeds off
}

void OBDHandle::checkECU() {
    clearCommandQueue();
    sendCommand("0100");
}

void OBDHandle::setServiceUUID(const char* uuid) { serviceUUID = BLEUUID(uuid); }
void OBDHandle::setCharUUID_TX(const char* uuid) { charUUID_TX = BLEUUID(uuid); }
void OBDHandle::setCharUUID_RX(const char* uuid) { charUUID_RX = BLEUUID(uuid); }
void OBDHandle::enableDebug(bool enable) { debugEnabled = enable; }
void OBDHandle::setDebugSerial(HardwareSerial* serial) { debugSerial = serial; }

void OBDHandle::addCommandToQueue(String command) {
    if (commandQueue.empty() && messageReceived == true) {
        sendCommand(command);
        return;
    }
    commandQueue.push(command);
}

void OBDHandle::clearCommandQueue() {
    while (!commandQueue.empty()) {
        commandQueue.pop();
    }
}