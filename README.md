# ESP32 OBD2 Reader

A real-time OBD2 data reader for ESP32 that connects to ELM327 Bluetooth devices to display vehicle diagnostics on an LCD screen.

## Features

- **Bluetooth Low Energy (BLE) Connection**: Connects to ELM327 v2.1 Chinese adapters
- **Real-time Data Display**: Shows RPM and engine temperature on a 16x2 LCD
- **Modular Architecture**: Clean separation between OBD handling and message processing
- **Debug Support**: Built-in serial debugging for troubleshooting

## Hardware Requirements

- ESP32 Development Board (ESP32 DOIT DEVKIT V1)
- 16x2 LCD with I2C backpack (address 0x27)
- ELM327 Bluetooth OBD2 Adapter (v2.1 Chinese version)
- Jumper wires and breadboard

## Wiring

| ESP32 Pin | Component | Notes |
|-----------|-----------|-------|
| GPIO 21 (SDA) | LCD SDA | I2C Data |
| GPIO 22 (SCL) | LCD SCL | I2C Clock |
| 3.3V | LCD VCC | Power |
| GND | LCD GND, LED GND | Ground |

## Software Dependencies

The project uses PlatformIO with the following libraries:
- `LiquidCrystal_I2C` - LCD display control
- Built-in ESP32 BLE libraries
- Arduino framework

## Configuration

Update the following variables in `main.cpp`:

```cpp
// Your ELM327 device MAC address
static String targetAddress = "66:1e:32:7a:35:0e";

// LCD I2C address (usually 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);
```

## Usage

1. **Setup**: The system initializes BLE and LCD on startup
2. **Connection**: Automatically attempts to connect to the ELM327 device
3. **ECU Detection**: Waits for vehicle ECU to become available
4. **Data Reading**: Continuously reads and displays:
   - RPM (revolutions per minute)
   - Engine coolant temperature

## Status Indicators

- **LED Blinking Fast**: Trying to connect to OBD2 adapter
- **LED Blinking Slow**: Connected to adapter, waiting for ECU
- **LED Solid/Off**: Normal operation, ECU active

## Project Structure

```
src/
├── main.cpp                 # Main application logic
lib/
├── OBDHandle/              # OBD2 communication handling
│   ├── obdhandle.h
│   └── obdhandle.cpp
├── MessageHandle/          # Message processing and display
│   ├── messagehandle.h
│   └── messagehandle.cpp
└── datadefinition.h        # Enums and data structures
```

## Key Classes

### OBDHandle
- Manages BLE connection to ELM327
- Handles OBD2 command sending/receiving
- Provides debug capabilities

### MessageHandle
- Processes OBD2 response messages
- Handles LCD display updates
- Parses RPM and temperature data

## Debug Mode

Enable debug output by uncommenting these lines in `setup()`:

```cpp
OBDHandle::setDebugSerial(&Serial);
OBDHandle::enableDebug(true);
```

This will output detailed connection and communication logs to the Serial Monitor.

## Supported OBD2 Commands

- `010C` - Engine RPM
- `0105` - Engine coolant temperature
- `0100` - ECU availability check

## Compatible Vehicles

This project works with vehicles that support:
- OBD2 standard (1996+ vehicles in most countries)
- ISO 9141-2 protocol (Ford Street protocol)

## Troubleshooting

1. **Connection Issues**: Verify ELM327 MAC address and ensure it's paired
2. **No ECU Response**: Check vehicle is running or in accessory mode
3. **LCD Not Working**: Verify I2C address with an I2C scanner
4. **Build Errors**: Ensure all library dependencies are installed

## License

This project is open source and available under the MIT License.

## Contributing

Feel free to submit issues, fork the repository, and create pull requests for any improvements.
