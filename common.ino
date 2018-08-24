
void WaitForSensorBootUpTime(int msec) {
  // waiting Milliseconds to BootUp the Sensor 
  // calculating since wakeUp
  if(millis() < starttime) {starttime = millis();} // überlaufen von millis() -> Rücksetzen auf 0
  if(millis() - starttime < msec) {
    delay(msec - (millis() - starttime)); 
  }
}

void Sensor_activate(boolean active) {
  if(active) {
    digitalWrite(SENSOR_ENABLE_PIN, HIGH);
    digitalWrite(OneWireData_Pin, HIGH);
  } else {
    digitalWrite(SENSOR_ENABLE_PIN, LOW);
    digitalWrite(OneWireData_Pin, LOW);
  }
}

void initSensors() {
  
  if(HAS_DS18B20 || HAS_DHT22 || HAS_BMP180 || HAS_TSL2561) {
    // nur aktivieren wenn wirklich benötigt
    Sensor_activate(true);
  }

  // Init all sensors
  initSensorAnalog();
    if(ENABLE_LED) { debug_LED(1,50,950,false); delay(1000);}
  initOneWire();
    if(ENABLE_LED) { debug_LED(1,50,950,false); delay(1000);}
  initSensorBMP180();
    if(ENABLE_LED) { debug_LED(1,50,950,false); delay(1000);}
  initSensorDHT();
    if(ENABLE_LED) { debug_LED(1,50,950,false); delay(1000);}
  initSensorTSL2561();
    if(ENABLE_LED) { debug_LED(1,50,950,false); delay(1000);}
    
  Sensor_activate(false);
}

/* shows any debug messages
   count      : count of flashing
   ontime     : how long the LED will be ON
   offtime    : how long the LED will be OFF
   showstart  : shows the beginning of this sequence with a short flash
*/

void debug_LED(unsigned int count, unsigned int ontime, unsigned int offtime, boolean showstart) {
  for(unsigned int i=1;i<=count;i++) {
    digitalWrite(LEDPIN, HIGH);
    delay(ontime);
    digitalWrite(LEDPIN, LOW);
    delay(offtime);
  }
}

void getVarsFromEeprom() {
  // Read periodic sync interval from EEPROM
  SyncInterval[0]       = EEPROM.read(EEPROM_SYNC_INTERVAL);
  SyncInterval[1]       = EEPROM.read(EEPROM_SYNC_INTERVAL + 1);

  SENDDELAY_MS[0]       = EEPROM.read(EEPROM_SENDDELAY_MS);
  SENDDELAY_MS[1]       = EEPROM.read(EEPROM_SENDDELAY_MS + 1);

  ALTITUDE[0]           = EEPROM.read(EEPROM_ALTITUDE);
  ALTITUDE[1]           = EEPROM.read(EEPROM_ALTITUDE + 1);
  
  DAYNIGHT_TYPE         = EEPROM.read(EEPROM_DAYNIGHT_TYPE);
  DAYNIGHT_THRESHOLD[0] = EEPROM.read(EEPROM_DAYNIGHT_THRESHOLD);
  DAYNIGHT_THRESHOLD[1] = EEPROM.read(EEPROM_DAYNIGHT_THRESHOLD + 1);
  DAYNIGHT_THRESHOLD[2] = EEPROM.read(EEPROM_DAYNIGHT_THRESHOLD + 2);
  DAYNIGHT_TX_INTERVAL[0] = EEPROM.read(EEPROM_DAYNIGHT_TX_INTERVAL);
  DAYNIGHT_TX_INTERVAL[1] = EEPROM.read(EEPROM_DAYNIGHT_TX_INTERVAL + 1);
  
  if (DAYNIGHT_TYPE == 0xFF) EEPROM.write(EEPROM_DAYNIGHT_TYPE, 0x00);
  
  static byte readEeprom;
  readEeprom            = EEPROM.read(EEPROM_USING_ANALOG);
  HAS_ANALOG4           = (readEeprom & 0x1);
  HAS_ANALOG3           = (readEeprom & 0x2) >> 0x1;
  HAS_ANALOG2           = (readEeprom & 0x4) >> 0x2;
  HAS_ANALOG1           = (readEeprom & 0x8) >> 0x3;
  
  readEeprom            = EEPROM.read(EEPROM_USING_SENSORS);
  HAS_DS18B20           = (readEeprom & 0x1);
  HAS_DHT22             = (readEeprom & 0x2) >> 0x1;
  HAS_TSL2561           = (readEeprom & 0x4) >> 0x2;
  HAS_BMP180            = (readEeprom & 0x8) >> 0x3;
  
  readEeprom            = EEPROM.read(EEPROM_TRANSMIT_VOLTAGE);
  TX_BATVOLT            = (readEeprom & 0x1);
  TX_SOLVOLT            = (readEeprom & 0x2) >> 0x1;
  ENABLE_LED            = (readEeprom & 0x4) >> 0x2;  
  ENABLE_DAYNIGHTSHIFT  = (readEeprom & 0x8) >> 0x3;
}

//http://playground.arduino.cc/code/AvailableMemory
int freeRam() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
