#include "accelerometerhandle.h"

static TwoWire accWire = TwoWire(1); // Use I2C bus 1 for the accelerometer

MPU6050 AccelerometerHandle::mpu(accWire);
bool AccelerometerHandle::initialized = false;
bool AccelerometerHandle::debugEnabled = false;
HardwareSerial* AccelerometerHandle::debugSerial = nullptr;
float AccelerometerHandle::currentPitch = 0.0f;
unsigned long AccelerometerHandle::lastUpdateTime = 0;
float AccelerometerHandle::filteredAccelX = 0.0f;

int AccelerometerHandle::voteCount = 0;
int AccelerometerHandle::positiveVotes = 0;
int AccelerometerHandle::negativeVotes = 0;

void AccelerometerHandle::begin() {
    if (!initialized) {
        accWire.begin(SENSOR_ACC_SDA_PIN, SENSOR_ACC_SCL_PIN);
        debugPrint("Attempting mpu.begin() on SDA=" + String(SENSOR_ACC_SDA_PIN) + " SCL=" + String(SENSOR_ACC_SCL_PIN));

        byte status = mpu.begin();
        if (status != 0) {
            debugPrint("Failed to find MPU6050 chip, status: " + String(status));
            while (1) {
                delay(100);
            }
        }

        setDLPF(2);

        debugPrint("MPU6050 Found!");

        debugPrint("Calculating offsets, keep the sensor still...");
        mpu.calcOffsets();
        debugPrint("Offsets calculated successfully");

        lastUpdateTime = millis();
        initialized = true;
    }
}

void AccelerometerHandle::updateCurrentVelocity() {
    if (!initialized) return;

    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;
    lastUpdateTime = currentTime;

    mpu.update();

    // Rough terrain: discard this reading, does not count as a vote
    float accelZ = mpu.getAccZ();
    float vibrationMagnitude = fabs(accelZ - 1.0f);

    if (vibrationMagnitude > VIBRATION_THRESHOLD) {
        debugPrint("Rough terrain detected, reading discarded (no vote)");
        return;
    }

    // Turn: discard this reading, does not count as a vote
    float yawRate = fabs(mpu.getGyroZ());

    if (yawRate > TURN_THRESHOLD) {
        debugPrint("Turn detected (yawRate=" + String(yawRate) + "), reading discarded");
        return;
    }

    currentPitch = mpu.getAngleY();

    float pitchRad = currentPitch * DEG_TO_RAD;
    float rawAccelX = mpu.getAccX() * GRAVITY_CONSTANT;
    filteredAccelX = ACCEL_FILTER_ALPHA * rawAccelX + (1 - ACCEL_FILTER_ALPHA) * filteredAccelX;
    float linearAcceleration = filteredAccelX + (GRAVITY_CONSTANT * sin(pitchRad));

    // Register this reading's vote
    if (linearAcceleration > ACCEL_DEADBAND) {
        positiveVotes++;
    } else if (linearAcceleration < -ACCEL_DEADBAND) {
        negativeVotes++;
    }
    // within deadband: doesn't count for either side, but still counts toward voteCount below

    voteCount++;

    debugPrint("Vote " + String(voteCount) + "/" + String(VOTES_NEEDED) +
               " | LinearAccel: " + String(linearAcceleration) +
               " | +:" + String(positiveVotes) + " -:" + String(negativeVotes));

    if (voteCount >= VOTES_NEEDED) {
        applyVote();
        voteCount = 0;
        positiveVotes = 0;
        negativeVotes = 0;
    }

    // Distance traveled is still updated on every valid reading,
    // using the most recently known (fused) speed
    float speedKmh = KalmanVelocity::getEstimate();
    double metersTraveled = (speedKmh / 3.6) * deltaTime;
    float totalKm = PreferencesHandle::getInstance().getDistanceTraveled() + (metersTraveled / 1000.0);
    PreferencesHandle::getInstance().setDistanceTraveled(totalKm);
}

void AccelerometerHandle::applyVote() {
    float currentEstimate = KalmanVelocity::getEstimate();
    float suggestedSpeed = currentEstimate;
    float uncertainty;

    int totalVotes = positiveVotes + negativeVotes;
    bool isUnanimous = (positiveVotes == 0 || negativeVotes == 0) && totalVotes > 0;
    uncertainty = isUnanimous ? UNANIMOUS_UNCERTAINTY : BASE_UNCERTAINTY;

    if (positiveVotes > negativeVotes) {
        suggestedSpeed = currentEstimate + NUDGE_AMOUNT;
        debugPrint("Majority: ACCELERATING (suggested=" + String(suggestedSpeed) +
                   ", uncertainty=" + String(uncertainty) + ")");
    } else if (negativeVotes > positiveVotes) {
        suggestedSpeed = currentEstimate - NUDGE_AMOUNT;
        debugPrint("Majority: BRAKING (suggested=" + String(suggestedSpeed) +
                   ", uncertainty=" + String(uncertainty) + ")");
    } else {
        debugPrint("Tie or neutral -> no nudge suggested, estimate unchanged");
        return; // não alimenta o Kalman se não há sinal de direção
    }

    if (suggestedSpeed < 0) {
        suggestedSpeed = 0;
    }

    float fusedSpeed = KalmanVelocity::update(suggestedSpeed, uncertainty);
    PreferencesHandle::getInstance().setVelocity(fusedSpeed);

    debugPrint("Fused speed = " + String(fusedSpeed) + " km/h");
}

void AccelerometerHandle::setDebugSerial(HardwareSerial* serial) {
    debugSerial = serial;
    debugEnabled = true;
}

void AccelerometerHandle::debugPrint(String message) {
    if (debugEnabled && debugSerial != nullptr) {
        debugSerial->println("[AccelerometerHandle] " + message);
    }
}

void AccelerometerHandle::setDLPF(uint8_t mode) {
    accWire.beginTransmission(0x68);
    accWire.write(0x1A);
    accWire.write(mode & 0x07);
    accWire.endTransmission();
}