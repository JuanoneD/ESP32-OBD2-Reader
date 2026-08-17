#include "gpshandle.h"

TinyGPSPlus GPSHandle::gps;
HardwareSerial GPSHandle::gpsSerial(2); // Serial2

bool GPSHandle::debugEnabled = false;
HardwareSerial* GPSHandle::debugSerial = nullptr;

unsigned long GPSHandle::lastFixTime = 0;
bool GPSHandle::initialized = false;

void GPSHandle::begin() {
    gpsSerial.begin(GPS_BAUD_RATE, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

    delay(100); // let the module stabilize before sending config

    setUpdateRate2Hz();

    initialized = true;
    debugPrint("GPS serial started on RX=" + String(GPS_RX_PIN) + " TX=" + String(GPS_TX_PIN) + " baud=" + String(GPS_BAUD_RATE));
}

// UBX-CFG-RATE: sets measurement rate to 500ms (2Hz), nav rate 1 cycle, time ref GPS
void GPSHandle::setUpdateRate2Hz() {
    uint8_t ubxRate2Hz[] = {
        0xB5, 0x62, 0x06, 0x08, 0x06, 0x00,
        0xF4, 0x01, // measRate = 500ms (little endian)
        0x01, 0x00, // navRate = 1 cycle
        0x01, 0x00, // timeRef = 1 (GPS time)
        0x0B, 0x77  // checksum (CK_A, CK_B)
    };

    gpsSerial.write(ubxRate2Hz, sizeof(ubxRate2Hz));
    debugPrint("UBX-CFG-RATE sent: requesting 2Hz update rate");
}

void GPSHandle::update() {
    if (!initialized) return;

    int bytesProcessed = 0;
    while (gpsSerial.available() > 0 && bytesProcessed < MAX_BYTES_PER_UPDATE) {
        gps.encode(gpsSerial.read());
        bytesProcessed++;
    }

    if (gps.location.isUpdated() && gps.location.isValid()) {
        lastFixTime = millis();
        debugPrint("Location updated: " + String(gps.location.lat(), 6) + ", " + String(gps.location.lng(), 6) +
                   " | Satellites: " + String(gps.satellites.value()));
    }

    if (gps.speed.isUpdated() && gps.location.isValid()) {
        double speedKmh = gps.speed.kmph();

        if (speedKmh < SPEED_DEADBAND_KMH) {
            speedKmh = 0.0;
        }

        float fusedSpeed = KalmanVelocity::update((float)speedKmh, GPS_UNCERTAINTY);
        PreferencesHandle::getInstance().setVelocity(fusedSpeed);

        debugPrint("GPS raw speed: " + String(speedKmh) + " km/h | Fused: " + String(fusedSpeed) + " km/h");
    }
}

bool GPSHandle::hasFix() {
    if (!gps.location.isValid()) return false;
    if (lastFixTime == 0) return false;
    return (millis() - lastFixTime) < FIX_TIMEOUT_MS;
}

double GPSHandle::getSpeedKmh() {
    if (!hasFix()) return -1.0;

    double speedKmh = gps.speed.kmph();
    if (speedKmh < SPEED_DEADBAND_KMH) {
        speedKmh = 0.0;
    }
    return speedKmh;
}

double GPSHandle::getLatitude() {
    return gps.location.lat();
}

double GPSHandle::getLongitude() {
    return gps.location.lng();
}

int GPSHandle::getSatellites() {
    if (!gps.satellites.isValid()) return 0;
    return gps.satellites.value();
}

void GPSHandle::setDebugSerial(HardwareSerial* serial) {
    debugSerial = serial;
}

void GPSHandle::enableDebug(bool enable) {
    debugEnabled = enable;
}

void GPSHandle::debugPrint(String message) {
    if (debugEnabled && debugSerial != nullptr) {
        debugSerial->println("[GPSHandle] " + message);
    }
}