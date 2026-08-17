#ifndef OBDHANDLE
#define OBDHANDLE

#include <Arduino.h>
#include <BLEDevice.h>
#include <messagehandle.h>
#include <queue>
#include <HardwareSerial.h>

class OBDHandle {
public:
    static bool begin();
    static bool connect(const char* address);
    static void update(); // chamar isso no loop() principal

    static void addCommandToQueue(String command);
    static void clearCommandQueue();
    static void checkECU();
    static void sendStarterCommand();

    static void setServiceUUID(const char* uuid);
    static void setCharUUID_TX(const char* uuid);
    static void setCharUUID_RX(const char* uuid);
    static void enableDebug(bool enable);
    static void setDebugSerial(HardwareSerial* serial);

private:
    static BLEUUID serviceUUID;
    static BLEUUID charUUID_TX;
    static BLEUUID charUUID_RX;
    static BLERemoteCharacteristic* pCharTX;
    static BLERemoteCharacteristic* pCharRX;
    static BLEClient* pClient;

    static String lastResponse;
    static bool messageReceived;
    static std::queue<String> commandQueue;

    static bool debugEnabled;
    static HardwareSerial* debugSerial;

    // Controle de envio (sem mutex, só flags + timer)
    static bool pendingSend;
    static String pendingCommand;
    static unsigned long lastCommandSentTime;
    static unsigned long currentTimeout;
    static const unsigned long DEFAULT_TIMEOUT = 1000;
    static const unsigned long AT_COMMAND_TIMEOUT = 3000;

    static void sendCommand(String command);
    static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
    static void debugPrint(String message);
};

#endif