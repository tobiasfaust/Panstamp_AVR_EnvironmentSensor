


/**
 * Sensor pins
 * D3 is a shared PowerUp-Pin for A2/A3 
 */
#define SENSOR_0_PIN  0    // A0 Analog pin - sensor 0
#define POWER_0_PIN   7    // D7 Digital pin used to powwer sensor 0

#define SENSOR_1_PIN  1    // A1 Analog pin - sensor 1
#define POWER_1_PIN   6    // D6 Digital pin used to powwer sensor 1

#define SENSOR_2_PIN  2    // A2 Analog pin - sensor 0
#define POWER_2_PIN   3    // D3 Digital pin used to powwer sensor 0

#define SENSOR_3_PIN  3    // A3 Analog pin - sensor 1
#define POWER_3_PIN   3    // D3 Digital pin used to powwer sensor 1

// Read Value from EEPROM in setup function
boolean HAS_ANALOG1;
boolean HAS_ANALOG2;
boolean HAS_ANALOG3;
boolean HAS_ANALOG4;
