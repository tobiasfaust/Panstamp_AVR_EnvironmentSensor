

/**
 * OneWire pin
 * Uncomment both if you are using a Dallas DS18x20
 */
#define OneWireData_Pin   8    // Digital pin for OneWireBus Data

// Read Value from EEPROM in setup function
boolean HAS_DS18B20;
unsigned int OneWireDevicesCount; // Anzahl an Gesamt Devices auf dem Bus
unsigned int OneWireDeviceCurrent; // Nummer des Devices welches aktuell gelesen werden soll
