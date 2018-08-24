#include <DHT.h>

// See guide for details on sensor wiring and usage:
// https://learn.adafruit.com/dht/overview

// Initialize the sensor
DHT dht(DHTPIN, DHTTYPE, 3);

void initSensorDHT(){
  if(HAS_DHT22) {
    #ifdef DEBUG_DHT22
      Serial.println("DHT Sensor Test");
    #endif
    
    dht.begin();
  
    /* Display some basic information on this sensor */
    //DHT_displaySensorDetails();
  }
}

/**************************************************************************/
/*
Displays some basic information on this sensor from the unified
sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
const void DHT_displaySensorDetails(void)
{
  // Print temperature sensor details.
  //sensor_t sensor;
  //dht.temperature().getSensor(&sensor);
    #ifdef DEBUG_DHT22
    //Serial.println("DHTxx Unified Sensor Example");
    //Serial.println("------------------------------------");
    //Serial.println("Temperature");
    //Serial.print ("Sensor: "); Serial.println(sensor.name);
    //Serial.print ("Driver Ver: "); Serial.println(sensor.version);
    //Serial.print ("Unique ID: "); Serial.println(sensor.sensor_id);
    //Serial.print ("Max Value: "); Serial.print(sensor.max_value); Serial.println(" *C");
    //Serial.print ("Min Value: "); Serial.print(sensor.min_value); Serial.println(" *C");
    //Serial.print ("Resolution: "); Serial.print(sensor.resolution); Serial.println(" *C");
    //Serial.println("------------------------------------");
    //Print humidity sensor details.
    //dht.humidity().getSensor(&sensor);
    //Serial.println("------------------------------------");
    //Serial.println("Humidity");
    //Serial.print ("Sensor: "); Serial.println(sensor.name);
    //Serial.print ("Driver Ver: "); Serial.println(sensor.version);
    //Serial.print ("Unique ID: "); Serial.println(sensor.sensor_id);
    //Serial.print ("Max Value: "); Serial.print(sensor.max_value); Serial.println("%");
    //Serial.print ("Min Value: "); Serial.print(sensor.min_value); Serial.println("%");
    //Serial.print ("Resolution: "); Serial.print(sensor.resolution); Serial.println("%");
    //Serial.println("------------------------------------");
    // Set delay between sensor readings based on sensor details.
  #endif
  //delayMS = sensor.min_delay / 1000;
}


/**
 * sensor_ReadDHT
 *
 * Read temperature and pressure from DHT sensor
 *
 */
const void sensor_ReadDHT(void) {
  if(HAS_DHT22) {
    //dht.begin();
    //delay(2000);
    
    // Delay between measurements.
    WaitForSensorBootUpTime(2000);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit
    // float f = dht.readTemperature(true);
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      #ifdef DEBUG_DHT22
        Serial.println("Failed to read from DHT sensor!");
      #endif
      dtTempHumSensor[0] = 0x00;
      dtTempHumSensor[1] = 0x00;
      dtTempHumSensor[2] = 0x00;
      dtTempHumSensor[3] = 0x00;
    } else {
      #ifdef DEBUG_DHT22
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.println(" *C");
        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.println("%");
      #endif  
      unsigned long tempValue = (t+50)*100; //keine negativen Zahlen (+50), keine Kommastelle (*100)
      dtTempHumSensor[0] = (tempValue >> 8) & 0xFF; // HighByte
      dtTempHumSensor[1] =  tempValue & 0xFF;       // LowByte
      
      unsigned long humValue = (h)*10; //keine Kommastelle (*10)
      dtTempHumSensor[2] = (humValue >> 8) & 0xFF; // HighByte
      dtTempHumSensor[3] =  humValue & 0xFF;       // LowByte
    }
  }
}
