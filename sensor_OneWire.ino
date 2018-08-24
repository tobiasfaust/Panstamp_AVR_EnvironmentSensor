
#include "OneWire.h"
#include "DallasTemperature.h"

/* Initialisieren der OneWire Library */
OneWire MyOneWire(OneWireData_Pin);
/* Dallas Temperature Library für Nutzung der oneWire Library vorbereiten */
DallasTemperature TemperatureSensors(&MyOneWire);


/**
 * Initialize OneWireBus
 * OneWireSearchDevices
 *
 * Initialisiert den OneWireBus und ermittelt alle Devices
 * http://blog.wenzlaff.de/?p=1254
 */
const void initOneWire(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  if(HAS_DS18B20){
    WaitForSensorBootUpTime(400);
    #ifdef DEBUG_1Wire
      Serial.print("Suche 1-Wire-Devices...\n\r");// "\n\r" is NewLine 
      while(MyOneWire.search(addr)) {
        Serial.print("\n\r\n\r1-Wire-Device gefunden mit Adresse:\n\r");
        for( i = 0; i < 8; i++) {
          Serial.print("0x");
          if (addr[i] < 16) {
            Serial.print('0');
          }
          Serial.print(addr[i], HEX);
          if (i < 7) {
            Serial.print(", ");
          }
        }
        if ( OneWire::crc8( addr, 7) != addr[7]) {
          Serial.print("CRC is not valid!\n\r");
          return;
        }
      }
      Serial.println();
      MyOneWire.reset_search();
    #endif
    
    /* Inizialisieren der Dallas Temperature library */
    TemperatureSensors.begin(); 
    OneWireDevicesCount = TemperatureSensors.getDeviceCount(); //Anzahl der Devices merken
    
    // Genauigkeit auf 10-Bit setzen
    TemperatureSensors.setResolution(TEMP_10_BIT); 
  }
  
  return;
}

/**
 * ReadOneWireTemp
 *
 * Measure sensor data and update register
 *
 */
const void sensor_ReadOneWireTemp()
{
  if(HAS_DS18B20){
    byte deviceAddress[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    float tempValue = 0;
    WaitForSensorBootUpTime(100);
    TemperatureSensors.requestTemperaturesByIndex(OneWireDeviceCurrent); // Temp abfragen
    tempValue = TemperatureSensors.getTempCByIndex(OneWireDeviceCurrent);
    TemperatureSensors.getAddress(deviceAddress, OneWireDeviceCurrent);
    unsigned int tempValue2 = (tempValue+50)*100; //keine negativen Zahlen (+50), keine Kommastelle (*100)
    #ifdef DEBUG
      Serial.print("Wert Temperatursensor (");
      for( byte i = 0; i < 8; i++) {
        Serial.print(deviceAddress[i],HEX); if(i<7) Serial.print(", ");
      }
      Serial.print("): ");
      Serial.print(tempValue,2);
      Serial.println(" Grad Celsius");
    #endif
    
    dt1WireSensor[0] = (tempValue2 >> 8) & 0xFF; // HighByte
    dt1WireSensor[1] =  tempValue2 & 0xFF;       // LowByte
    //1wireID übermitteln
    for( byte i = 0; i < 8; i++) {
      dt1WireSensor[i+2] = deviceAddress[i];
    } 
  } else {
    dt1WireSensor[0] = 0x00;
    dt1WireSensor[1] = 0x00;
  }
}

