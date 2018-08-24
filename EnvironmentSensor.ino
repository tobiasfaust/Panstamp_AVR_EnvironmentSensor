/*
 * EnvironmentSensor
 *
 * Copyright (c) 2011 Daniel Berenguer <dberenguer@usapiens.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 * 
 * panStamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with panStamp; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 04/29/2013
 *
 * Device:
 * Soil Moisture sensor
 *
 * Description:
 * This application measures soil moisture from any two sensor providing an
 * analog signal
 *
 * These devices are low-power enabled so they will enter low-power mode
 * just after reading the sensor values and transmitting them over the
 * SWAP network.
 *
 * Associated Device Definition File, defining registers, endpoints and
 * configuration parameters:
 * soilmoisture.xml
 */
 
#include "regtable.h"
#include "panstamp.h"
#include "common.h"
#include "analogsensor.h"
#include "sensor_bmp180.h"
#include "sensor_DHT22.h"
#include "sensor_TSL2561.h"
#include "sensor_OneWire.h"

/* Uncomment if you want debugmessages over serial (38400 Baud) */
//#define DEBUG
//#define DEBUG_DHT22
//#define DEBUG_1Wire

/**
 * Uncomment if you want Batterie-/Solarvoltage
 */
#define VOLT_SUPPLY   6
#define SOLAR_SUPPLY  7

/**
 * LED pin
 */
#define LEDPIN        4


static unsigned long int slept = 0;
static unsigned long last_sync_time = 0;
bool isTime(unsigned long &timeMark, unsigned long timeInterval){
  if( millis() + slept - timeMark >= timeInterval )
    {   
      timeMark = millis() + slept;
      return true;
    }   
   return false;
}

void syncState(){
  // Enter SYNC state
  #ifdef DEBUG
    Serial.print("Starte Sync......");
  #endif
  panstamp.enterSystemState(SYSTATE_SYNC);
  // During 3 seconds, listen the network for possible commands whilst the LED is on
  if(ENABLE_LED) {
    debug_LED(1, 3000,1000, false);
  }
  // Switch to Rx OFF state
  panstamp.enterSystemState(SYSTATE_RXOFF);
  #ifdef DEBUG
    Serial.println("Sync beendet");
  #endif
}

/**
 * setup
 *
 * Arduino setup function
 */
void setup()
{
  //EEPROM.write(0x09, 0x09);
  
  //EEPROM.write(EEPROM_TX_INTERVAL, 0x55);
  //EEPROM.write(EEPROM_TX_INTERVAL+1, 0x55);   
  
  starttime = millis();

  #ifdef DEBUG_1Wire
    Serial.begin(38400);
    Serial.println("Starte Debugausgabe");
  #endif
 
  getVarsFromEeprom();
  
  // Init panStamp
  panstamp.init();

  //TODO: pruefen von sich ausschließenden Kombinationen
  //if(HAS_DHT22) HAS_ANALOG1=0;
  //HAS_ANALOG1 = 1;
  //HAS_ANALOG2 = 1;
  //HAS_ANALOG3 = 1;
  //HAS_ANALOG4 = 1;
  //HAS_BMP180 = 0;
  //HAS_TSL2561= 0;
  //HAS_DHT22  = 0;
  //HAS_DS18B20= 1;
  //TX_BATVOLT = 1;
  //TX_SOLVOLT = 1;
  //ENABLE_LED = 1;
  
  #ifdef DEBUG
    Serial.print("SyncInterval: ");
    Serial.print(SyncInterval[0] , HEX);
    Serial.println(SyncInterval[1], HEX);
    Serial.print("HAS_ANALOG1    : "); Serial.println(HAS_ANALOG1, DEC);
    Serial.print("HAS_ANALOG2    : "); Serial.println(HAS_ANALOG2, DEC);
    Serial.print("HAS_ANALOG3    : "); Serial.println(HAS_ANALOG3, DEC);
    Serial.print("HAS_ANALOG4    : "); Serial.println(HAS_ANALOG4, DEC);
    Serial.print("HAS_DS18B20    : "); Serial.println(HAS_DS18B20, DEC);
    Serial.print("HAS_DHT22      : "); Serial.println(HAS_DHT22  , DEC);
    Serial.print("HAS_TSL2561    : "); Serial.println(HAS_TSL2561, DEC);
    Serial.print("HAS_BMP180     : "); Serial.println(HAS_BMP180 , DEC);
    Serial.print("TX_BATVOLT     : "); Serial.println(TX_BATVOLT , DEC);
    Serial.print("TX_SOLVOLT     : "); Serial.println(TX_SOLVOLT , DEC);
  #endif
  
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  
  pinMode(VOLT_SUPPLY,  INPUT);
  pinMode(SOLAR_SUPPLY, INPUT);

  pinMode(SENSOR_ENABLE_PIN, OUTPUT);
  digitalWrite(SENSOR_ENABLE_PIN, LOW); //initialize
    
  pinMode(OneWireData_Pin, OUTPUT);
  digitalWrite(OneWireData_Pin, LOW);
  
  // Transmit product code
  getRegister(REGI_PRODUCTCODE)->getData();

  // Enter SYNC state
  syncState();

  delay(2000);
  
  // Transmit periodic Tx interval
  getRegister(REGI_TXINTERVAL)->getData();
  // Transmit Configuration
  getRegister(REGI_CONFIGURATION)->getData();
  // Transmit power voltage
  getRegister(REGI_VOLTSUPPLY)->getData();
  
  // Switch to Rx OFF state
  panstamp.enterSystemState(SYSTATE_RXOFF);
  
  initSensors();
  
  delay(5000);
}

/**
 * loop
 *
 * Arduino main loop
 */
void loop() {
  boolean FirstRegAlreadyRun = false; 
  starttime = millis();

  if(HAS_DS18B20 || HAS_DHT22 || HAS_BMP180 || HAS_TSL2561) {
    // nur aktivieren wenn wirklich benötigt
    Sensor_activate(true);
  }
  
  #ifdef DEBUG 
    Serial.println("------------------------------------------------");
    Serial.println("Panstamp WakeUp.....");
  #endif
  
  // Transmit AnalogSensor Register
  if(HAS_ANALOG1 || HAS_ANALOG2 || HAS_ANALOG3 || HAS_ANALOG4) {
    #ifdef DEBUG 
      Serial.println("Panstamp Aktualisiere AnalogSensor");  
    #endif
    if(FirstRegAlreadyRun) delay(SENDDELAY_MS[0] << 8 | SENDDELAY_MS[1]); //nur warten falls hiervor schon ein Register gesendet wurde
    getRegister(REGI_ANALOGSENSOR)->getData();
    FirstRegAlreadyRun=true;
  }
  
  // Transmit 1Wire Register
  if(HAS_DS18B20) {
    #ifdef DEBUG_1Wire 
      Serial.println("Panstamp Aktualisiere DS18B20");
      Serial.print  (OneWireDevicesCount);
      Serial.println(" x Devices werden aktualisiert");  
    #endif
    for(unsigned int c=0; c < OneWireDevicesCount; c++) {
      OneWireDeviceCurrent = c;
      if(FirstRegAlreadyRun) delay(SENDDELAY_MS[0] << 8 | SENDDELAY_MS[1]); //nur warten falls hiervor schon ein Register gesendet wurde
      getRegister(REGI_1WIRE)->getData();
      FirstRegAlreadyRun=true;
    }
  }
  
  // Transmit TempHum DHT22 Register
  if(HAS_DHT22) {
    #ifdef DEBUG_DHT22 
      Serial.println("Panstamp Aktualisiere DHT22");  
    #endif
    if(FirstRegAlreadyRun) delay(SENDDELAY_MS[0] << 8 | SENDDELAY_MS[1]); //nur warten falls hiervor schon ein Register gesendet wurde
    getRegister(REGI_TEMPHUM)->getData();
    FirstRegAlreadyRun=true;
  }
  
  // Transmit Press Register
  if(HAS_BMP180) {
    #ifdef DEBUG 
      Serial.println("Panstamp Aktualisiere BMP180");  
    #endif
    if(FirstRegAlreadyRun) delay(SENDDELAY_MS[0] << 8 | SENDDELAY_MS[1]); //nur warten falls hiervor schon ein Register gesendet wurde
    getRegister(REGI_PRESS)->getData();
    FirstRegAlreadyRun=true;
  }
  
  // Transmit Light Register
  if(HAS_TSL2561) {
    #ifdef DEBUG 
      Serial.println("Panstamp Aktualisiere TSL2561");  
    #endif
    if(FirstRegAlreadyRun) delay(SENDDELAY_MS[0] << 8 | SENDDELAY_MS[1]); //nur warten falls hiervor schon ein Register gesendet wurde
    getRegister(REGI_LUM)->getData();
    FirstRegAlreadyRun=true;
  }
  
  // deaktivieren von VCC für alle Sensoren
  Sensor_activate(false);
 
  // Transmit power voltage
  if(TX_BATVOLT || TX_SOLVOLT) {
    #ifdef DEBUG
      Serial.println("Panstamp Aktualisiere Batterie/SolarWerte");
    #endif
    if(FirstRegAlreadyRun) delay(SENDDELAY_MS[0] << 8 | SENDDELAY_MS[1]); //nur warten falls hiervor schon ein Register gesendet wurde
    getRegister(REGI_VOLTSUPPLY)->getData();
    FirstRegAlreadyRun=true;
  }

  if(ENABLE_LED) {
      debug_LED(1, 50,100, false);
      delay(800);
  }
  
  #ifdef DEBUG 
    Serial.println("Panstamp GoToSleep.....");
  #endif
  
  if(((SyncInterval[0] << 8 | SyncInterval[1]) > 0) && isTime(last_sync_time, (unsigned long)1000 * (SyncInterval[0] << 8 | SyncInterval[1]))) 
      syncState();
    

  //wenn Day/NightShifting, vergleiche Schwellwert und setze nächsten Aufwachzeitpunkt
  if(ENABLE_DAYNIGHTSHIFT) {
    static byte TX_INTERVAL[2];
    TX_INTERVAL[0] = EEPROM.read(EEPROM_TX_INTERVAL);
    TX_INTERVAL[1] = EEPROM.read(EEPROM_TX_INTERVAL + 1);
    boolean isDay = false;
    
    if(DAYNIGHT_TYPE == 1 && TX_SOLVOLT) {
      //Solar
      if((dtVoltSupply[2] << 8 | dtVoltSupply[3]) > (DAYNIGHT_THRESHOLD[0] << 8 | DAYNIGHT_THRESHOLD[1])) {
        //SolarSpannung in mV ist größer als Schwellwert
        isDay=true;
      }
    } else if(DAYNIGHT_TYPE == 2 && HAS_TSL2561) {
      //TSL2561
      if((dtLumSensor[0] << 24 | dtLumSensor[1] << 16 | dtLumSensor[2] << 8 | dtLumSensor[3]) > (DAYNIGHT_THRESHOLD[0] << 16 | DAYNIGHT_THRESHOLD[1] << 8 | DAYNIGHT_THRESHOLD[2])) {
        //Luminousity ist größer als Schwellwert
        isDay=true;
      }
    }
    
    if(isDay) {
      //panstamp.setTxInterval(DAYNIGHT_TX_INTERVAL,0);
      //panstamp.txInterval[0] = DAYNIGHT_TX_INTERVAL[0];
      //panstamp.txInterval[1] = DAYNIGHT_TX_INTERVAL[1];
      panstamp.txInterval = (DAYNIGHT_TX_INTERVAL[0] << 8) | DAYNIGHT_TX_INTERVAL[1];
    } else {
      //panstamp.txInterval[0] = TX_INTERVAL[0];
      //panstamp.txInterval[1] = TX_INTERVAL[1];  
      panstamp.txInterval = (TX_INTERVAL[0] << 8) | TX_INTERVAL[1];    
    }      
  } 
   
  //delay(10000);
  // Sleep
  panstamp.goToSleep();
  //slept += (unsigned long)1000 * (panstamp.txInterval[0] << 8 | panstamp.txInterval[1]);
  slept += (unsigned long)1000 * panstamp.txInterval;
}

