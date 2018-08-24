
boolean TX_BATVOLT;
boolean TX_SOLVOLT;
boolean ENABLE_LED;
boolean ENABLE_DAYNIGHTSHIFT;

static byte SENDDELAY_MS[2];
static byte ALTITUDE[2];
static byte DAYNIGHT_TYPE;
static byte DAYNIGHT_THRESHOLD[3];
static byte DAYNIGHT_TX_INTERVAL[2];

unsigned long starttime = millis();

void debug_LED(unsigned int count, unsigned int ontime, unsigned int offtime, boolean showstart);

void getVarsFromEeprom();
int freeRam();

#define SENSOR_ENABLE_PIN    9

