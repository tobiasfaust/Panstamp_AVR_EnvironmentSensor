
void initSensorAnalog(){
  // Initialize power pins
  if(HAS_ANALOG1) {
    pinMode(POWER_0_PIN, OUTPUT);
    digitalWrite(POWER_0_PIN, LOW);
  }
  
  if(HAS_ANALOG2) {
    pinMode(POWER_1_PIN, OUTPUT);
    digitalWrite(POWER_1_PIN, LOW);
  }
  
  if(HAS_ANALOG3) {
    pinMode(POWER_2_PIN, OUTPUT);
    digitalWrite(POWER_2_PIN, LOW);
  }
  
  if(HAS_ANALOG4) {
    pinMode(POWER_3_PIN, OUTPUT);
    digitalWrite(POWER_3_PIN, LOW);
  }
}

/**
 * sensor_ReadAnalog
 *
 * Measure sensor data and update register
 *
 */
const void sensor_ReadAnalog()
{
  unsigned int adcValue = 0;
  uint8_t POWER_PIN[4];
  uint8_t SENSOR_PIN[4];
  
    POWER_PIN[0]  = POWER_0_PIN;
    SENSOR_PIN[0] = SENSOR_0_PIN;
 
    POWER_PIN[1] = POWER_1_PIN;
    SENSOR_PIN[1] = SENSOR_1_PIN;
  
    POWER_PIN[2] = POWER_2_PIN;
    SENSOR_PIN[2] = SENSOR_2_PIN;
  
    POWER_PIN[3] = POWER_3_PIN;
    SENSOR_PIN[3] = SENSOR_3_PIN;
  
  for(int i=0 ; i<=3 ; i++) {
    if ((i==0 && HAS_ANALOG1) || \
        (i==1 && HAS_ANALOG2) || \
        (i==2 && HAS_ANALOG3) || \
        (i==3 && HAS_ANALOG4) ) {
      // Power up sensor
      digitalWrite(POWER_PIN[i], HIGH);
      delay(400); // mindestens 400ms bis der Vegetronix VH400 ein korrektes Ergebnis liefern kann
      // Read analog value
      adcValue = analogRead(SENSOR_PIN[i]);
      // Unpower sensor
      digitalWrite(POWER_PIN[i], LOW);
      // Update register value
      //Serial.println(char('Wert Sensor ' + i + ': ' + adcValue));
      #ifdef DEBUG
        Serial.print("Wert des Sensors "); Serial.print((i+1),DEC); Serial.print(": "); Serial.println(adcValue,HEX);
      #endif
      dtAnalogSensor[(i*2)]   = (adcValue >> 8) & 0xFF;
      dtAnalogSensor[(i*2)+1] = adcValue & 0xFF;
    } else {
      dtAnalogSensor[(i*2)]   = 0x00;
      dtAnalogSensor[(i*2)+1] = 0x00;
    }      
  }
}

