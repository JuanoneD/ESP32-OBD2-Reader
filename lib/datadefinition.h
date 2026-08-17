#ifndef DATADEFINITION
#define DATADEFINITION

// --- OBD-II MUX Constants ---
#define CHECK_ECU_MUX 0x00
#define RPM_MUX 0x0C
#define TEMP_MUX 0x05
#define ENGINE_LOAD_MUX 0x04
#define SPEED_MUX 0x0D
#define LONG_TERM_FUEL_TRIM_MUX 0x07

#define PREFERENCE_NAMESPACE "OBD2_READER"

// Pin definitions for the ESP32
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22
#define SENSOR_ACC_SDA_PIN 23
#define SENSOR_ACC_SCL_PIN 19
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

// --- Configuration Constants ---
#define SERIAL_BAUD_RATE      115200
#define GPS_BAUD_RATE         9600
#define SENSOR_INTERVAL_MS    20       // 50 Hz loop frequency (20 milliseconds)
#define GRAVITY_CONSTANT      9.81     // m/s^2
#define DEFAULT_MESSAGE_INTERVAL_MS 200
#define MEDIUM_DELAY_MESSAGE_INTERVAL_MS 1000  // Interval for medium delay messages (1 second)
#define LONG_DELAY_MESSAGE_INTERVAL_MS 10000  // Interval for long delay messages (10 seconds)
#define ACCELEROMETER_UPDATE_INTERVAL_MS 20  // Interval for accelerometer updates
#define UPDATED_LCD_INTERVAL_MS 100  // Interval for updating the LCD display
#define ACCEL_FILTER_ALPHA 0.05f
#define ACCEL_DEADBAND 0.3f
#define VIBRATION_THRESHOLD 0.1f
#define TURN_THRESHOLD 5.0f
#define MAX_DELTA_V_PER_CYCLE 3.0f
#define NUDGE_AMOUNT 1.0f
#define VOTES_NEEDED 5
#define MAX_BYTES_PER_UPDATE 64
#define SPEED_DEADBAND_KMH 2.0f
#define FIX_TIMEOUT_MS 5000
// Filter coefficients
#define COMPLEMENTARY_ALPHA   0.98     // Gyroscope weight
#define COMPLEMENTARY_BETA    0.02     // Accelerometer weight

// Conversion factors
#define RAD_TO_DEG            57.2957795
#define DEG_TO_RAD            0.01745329
#define MS_TO_KMH             3.6


//Kalman filter constants
#define BASE_UNCERTAINTY 8.0f
#define UNANIMOUS_UNCERTAINTY 5.0f
#define GPS_UNCERTAINTY  2.0f


enum class CONNECTION_STATUS {
    DISCONNECTED,
    CONNECTED
};

enum class ECU_STATUS {
    SLEEP,
    AWAKE
};

#endif