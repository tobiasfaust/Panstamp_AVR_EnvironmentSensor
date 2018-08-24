
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
  which provides a common 'type' for sensor data and some helper functions.
  To use this driver you will also need to download the Adafruit_Sensor
  library and include it in your libraries folder.
  You should also assign a unique ID to this sensor for use with
  the Adafruit Sensor API so that you can identify this particular
  sensor in any data logs, etc. To assign a unique ID, simply
  provide an appropriate value in the constructor below (12345
  is used by default in this example).
  Connections
  ===========
  Connect SCL to analog 5
  Connect SDA to analog 4
  Connect VDD to 3.3V DC
  Connect GROUND to common ground
  I2C Address
  ===========
  The address will be different depending on whether you leave
  the ADDR pin floating (addr 0x39), or tie it to ground or vcc.
  The default addess is 0x39, which assumes the ADDR pin is floating
  (not connected to anything). If you set the ADDR pin high
  or low, use TSL2561_ADDR_HIGH (0x49) or TSL2561_ADDR_LOW
  (0x29) respectively.
  History
  =======
  2013/JAN/31 - First version (KTOWN)
*/

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

/**************************************************************************/
/*
  Arduino Init function (called at startup)
*/
/**************************************************************************/
void initSensorTSL2561(void)
{
  if(HAS_TSL2561) {
    #ifdef DEBUG
      Serial.println("Light Sensor Test"); Serial.println("");
    #endif
    
    WaitForSensorBootUpTime(100); 
    
    if(!tsl.begin())
    {
      /* There was a problem detecting the ADXL345 ... check your connections */
      #ifdef DEBUG
        Serial.print("Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!");
      #endif
      HAS_TSL2561=0;
      debug_LED(3, 150,400, false);
    } else {
      /* Display some basic information on this sensor */
      TSL2561_displaySensorDetails();
      /* Setup the sensor gain and integration time */
      TSL2561_configureSensor();
      /* We're ready to go! */
      #ifdef DEBUG
        Serial.println("TSL2561 erkannt");
      #endif
    }
  }
}

/**************************************************************************/
/*
  Displays some basic information on this sensor from the unified
  sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void TSL2561_displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  #ifdef DEBUG
    Serial.println("------------------------------------");
    Serial.print ("Sensor: "); Serial.println(sensor.name);
    Serial.print ("Driver Ver: "); Serial.println(sensor.version);
    Serial.print ("Unique ID: "); Serial.println(sensor.sensor_id);
    Serial.print ("Max Value: "); Serial.print(sensor.max_value); Serial.println(" lux");
    Serial.print ("Min Value: "); Serial.print(sensor.min_value); Serial.println(" lux");
    Serial.print ("Resolution: "); Serial.print(sensor.resolution); Serial.println(" lux");
    Serial.println("------------------------------------");
    Serial.println("");
  #endif
  WaitForSensorBootUpTime(500);
}
/**************************************************************************/
/*
  Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void TSL2561_configureSensor(void)
{
    /* You can also manually set the gain or enable auto-gain support */
    // tsl.setGain(TSL2561_GAIN_1X); /* No gain ... use in bright light to avoid sensor saturation */
    // tsl.setGain(TSL2561_GAIN_16X); /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true); /* Auto-gain ... switches automatically between 1x and 16x */
    /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS); /* fast but low resolution */
    // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS); /* medium resolution and speed */
    // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS); /* 16-bit data but slowest conversions */
    /* Update these values depending on what you've set above! */
  #ifdef DEBUG
    Serial.println("------------------------------------");
    Serial.print ("Gain: "); Serial.println("Auto");
    Serial.print ("Timing: "); Serial.println("13 ms");
    Serial.println("------------------------------------");
  #endif
}


/**************************************************************************/
/*
  Main Function
*/
/**************************************************************************/
void sensor_ReadTSL2561() {
  if(HAS_TSL2561) {
    WaitForSensorBootUpTime(100); 
    
    tsl.begin();
    
    /* Get a new sensor event */
    sensors_event_t event;
    tsl.getEvent(&event);
      /* Display the results (light is measured in lux) */
    if (event.light){
      #ifdef DEBUG
        Serial.print(event.light); Serial.println(" lux");
      #endif
      
      unsigned long lumValue = (event.light); 
      dtLumSensor[0] = (lumValue >> 24) & 0xFF; 
      dtLumSensor[1] = (lumValue >> 16) & 0xFF; 
      dtLumSensor[2] = (lumValue >> 8) & 0xFF; 
      dtLumSensor[3] =  lumValue & 0xFF;
      
    } else  {
      /* If event.light = 0 lux the sensor is probably saturated
         and no reliable data could be generated! */
      #ifdef DEBUG
        Serial.println("Sensor overload");
      #endif
    }
  } else {
    dtLumSensor[0] = 0x00;
    dtLumSensor[1] = 0x00;
    dtLumSensor[2] = 0x00;
    dtLumSensor[3] = 0x00;
  }
}
