

#include <Wire.h>
#include "Adafruit_Sensor.h"
#include "Adafruit_BMP085_U.h"

/* Initialisieren des BPMP185 */
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void initSensorBMP180(){
  if(HAS_BMP180) {
    #ifdef DEBUG
      Serial.println("Pressure Sensor Test"); Serial.println("");
    #endif
    /* Initialise the sensor */
    WaitForSensorBootUpTime(10);
    if(!bmp.begin()) {
      /* There was a problem detecting the BMP085 ... check your connections */
      #ifdef DEBUG
        Serial.print("Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!");
      #endif
      HAS_BMP180=0;
      debug_LED(4, 150,400, false);
    } else {
      /* Display some basic information on this sensor */
      BMP180_displaySensorDetails();
    }
  }
}

/**************************************************************************/
/*
Displays some basic information on this sensor from the unified
sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
const void BMP180_displaySensorDetails(void)
{
  sensor_t sensor;
  bmp.getSensor(&sensor);
  #ifdef DEBUG
    Serial.println("------------------------------------");
    Serial.print ("Sensor: "); Serial.println(sensor.name);
    Serial.print ("Driver Ver: "); Serial.println(sensor.version);
    Serial.print ("Unique ID: "); Serial.println(sensor.sensor_id);
    Serial.print ("Max Value: "); Serial.print(sensor.max_value); Serial.println(" hPa");
    Serial.print ("Min Value: "); Serial.print(sensor.min_value); Serial.println(" hPa");
    Serial.print ("Resolution: "); Serial.print(sensor.resolution); Serial.println(" hPa");
    Serial.println("------------------------------------");
    Serial.println("");
  #endif
  WaitForSensorBootUpTime(500);
}

/**
 * sensor_ReadTempPress
 *
 * Read temperature and pressure from BMP085 sensor
 *
 * Return -1 in case of error. Return 0 otherwise
 */
const void sensor_ReadBMP180(void) {
  if(HAS_BMP180) {
    WaitForSensorBootUpTime(400);
    
    /* Get a new sensor event */
    sensors_event_t event;
    bmp.getEvent(&event);
    /* Display the results (barometric pressure is measure in hPa) */
    if (event.pressure)
    {
      /* Display atmospheric pressue in hPa */
      #ifdef DEBUG
        Serial.print("Pressure: ");
        Serial.print(event.pressure);
        Serial.println(" hPa");
      #endif
      
      unsigned long pressValue = (event.pressure); 
      //pressValue = pressValue * 100;
      dtPressSensor[2] = ((pressValue * 100) >> 24) & 0xFF; 
      dtPressSensor[3] = ((pressValue * 100) >> 16) & 0xFF; 
      dtPressSensor[4] = ((pressValue * 100) >> 8) & 0xFF; 
      dtPressSensor[5] =  (pressValue * 100) & 0xFF;
      
        /* Calculating altitude with reasonable accuracy requires pressure *
        * sea level pressure for your position at the moment the data is *
        * converted, as well as the ambient temperature in degress *
        * celcius. If you don't have these values, a 'generic' value of *
        * 1013.25 hPa can be used (defined as SENSORS_PRESSURE_SEALEVELHPA *
        * in sensors.h), but this isn't ideal and will give variable *
        * results from one day to the next. *
        * *
        * You can usually find the current SLP value by looking at weather *
        * websites or from environmental information centers near any major *
        * airport. *
        * *
        * For example, for Paris, France you can check the current mean *
        * pressure and sea level at: http://bit.ly/16Au8ol */
        /* First we get the current temperature from the BMP085 */
      float temperature;
      bmp.getTemperature(&temperature);
      
      #ifdef DEBUG
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" C");
      #endif
      
      unsigned int tempValue = (temperature+50)*100; //keine negativen Zahlen (+50), keine Kommastelle (*100)
      dtPressSensor[0] = (tempValue >> 8) & 0xFF; // HighByte
      dtPressSensor[1] =  tempValue & 0xFF;       // LowByte      
      
      // Computing the SeaLevelPressure with the altitude of your current location in meter
      unsigned long seaLevelPressure = bmp.seaLevelForAltitude(((ALTITUDE[0] << 8) | ALTITUDE[1] ), pressValue);
      
      #ifdef DEBUG
        Serial.print("SeaLevelPressure: ");
        Serial.print(seaLevelPressure);
        Serial.println(" hPa");
        Serial.println("");
      #endif
      
      dtPressSensor[6] = ((seaLevelPressure*100) >> 24) & 0xFF; 
      dtPressSensor[7] = ((seaLevelPressure*100) >> 16) & 0xFF; 
      dtPressSensor[8] = ((seaLevelPressure*100) >> 8) & 0xFF; 
      dtPressSensor[9] =  (seaLevelPressure*100) & 0xFF;
    }
      else {
      #ifdef DEBUG
        Serial.println("Sensor error");
      #endif
    }
  } else {
    dtPressSensor[0] = 0x00;
    dtPressSensor[1] = 0x00;
    dtPressSensor[2] = 0x00;
    dtPressSensor[3] = 0x00;
    dtPressSensor[4] = 0x00;
    dtPressSensor[5] = 0x00;
    dtPressSensor[6] = 0x00;
    dtPressSensor[7] = 0x00;
    dtPressSensor[8] = 0x00;
    dtPressSensor[9] = 0x00;
  }
}
