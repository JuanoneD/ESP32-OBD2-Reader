#ifndef DATADEFINITION
#define DATADEFINITION

#define RPM_MUX 0x0C
#define TEMP_MUX 0x05

enum class CONNECTION_STATUS {
    DISCONNECTED,
    CONNECTED
};

enum class ECU_STATUS {
    SLEEP,
    AWAKE
};

#endif