/**
 * regtable.h
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
 */

#ifndef _REGTABLE_H
#define _REGTABLE_H

#include "Arduino.h"
#include "register.h"
#include "commonregs.h"

/*
zb: 0B A8C0 0064 0035 01 03E8 003C 0C 01 0B 
-> Sync alle 12Stunden, 100ms Verzögerung, 54m Alt, Type1, 1000mV, 60s, 2x Analog, DS18B20, BatVolt, SolVolt, DayNightShifting)
Ist Sync=0000, dann deaktiviert

 * Definition of Configuration 
   Syncing Interval            2 Byte | Position Byte 0+1        // Intervall für den SyncStatus zur Zentrale 
   SendDelay                   2 Byte | Position Byte 2+3        // Sendeverzögerung in ms zwischen dem Senden von mehreren Registern. Soll Sendeüberlagerung bei Nutzung von Repeatern minimieren
   Altitude                    2 Byte | Position Byte 4+5        // Benutzt in bmp180.ino zur Berechnung von SeaLevelPressure
   Day/NightShift Type         1 Byte | Position Byte 6          // Type:1 -> Nutzung Solarwert | Type:2 -> Nutzung TSL2561
   Day/NightShift Threshold    3 Byte | Position Byte 7+8+9      // Schwellwert bei dem Day/Night umgeschaltet werden soll, Type 1: in mV |Type 2: in Lum
   Day/NightShift TX Interval  2 Byte | Position Byte 10+11      // Intervall im Tagesrhythmus, Das "normale" TX_INTERVAL ist dann NachtRhythmus
   
   HAS_Analog4                 1 Bit  | Position Byte 12.0
   HAS_Analog3                 1 Bit  | Position Byte 12.1
   HAS_Analog2                 1 Bit  | Position Byte 12.2
   HAS_Analog1                 1 Bit  | Position Byte 12.3
   
   HAS_DS18B20                 1 Bit  | Position Byte 13.0
   HAS_DHT22                   1 Bit  | Position Byte 13.1
   HAS_TSL2561                 1 Bit  | Position Byte 13.2
   HAS_BMP180                  1 Bit  | Position Byte 13.3
   
   Transmit BatteryVoltage     1 Bit  | Position Byte 14.0
   Transmit SolarVoltage       1 Bit  | Position Byte 14.1 
   Enable Debug LED            1 Bit  | Position Byte 14.2
   Enable Day/Night Shift      1 Bit  | Position Byte 14.3
*/
static byte dtConfiguration[15];

/* Battery & SolarVoltage supply
  Battery Voltage            2 Byte
  Solar Voltage              2 Byte
*/
static byte dtVoltSupply[4];                         // Data container: two 10-bit ADC values

/* Sensor value register (quad sensor)
  Analog Value Sensor 0      2 Byte
  Analog Value Sensor 1      2 Byte
  Analog Value Sensor 2      2 Byte
  Analog Value Sensor 3      2 Byte
*/
static byte dtAnalogSensor[8];


/* 1Wire Temperature value register (1Wire Sensor)
  Temperature OneWire        2 Byte
  1wireAddress               8 Byte
*/
static byte dt1WireSensor[10]; 

/* Temperature & Humidity Sensor value register (DHT22 Sensor)
  Temperature DHT            2 Byte
  Humidity DHT               2 Byte
*/
static byte dtTempHumSensor[4]; 

/* Pressure Sensor value register (BMP185 Sensor)
  Temperature                2 Byte
  Pressure                   4 Byte
  SeaLevelPressure           4 Byte
*/
static byte dtPressSensor[10]; 

/* Lumisity Sensor value register (TSL2561 Sensor)
  Lumiusity                  4 Byte
*/
static byte dtLumSensor[4]; 



/*
 * Definition of Syncing Interval in PowerDownMode 
 */
static byte SyncInterval[2];
#define EEPROM_SYNC_INTERVAL        0x20  //2-byte register
#define EEPROM_USING_ANALOG         0x22  //1-byte register
#define EEPROM_USING_SENSORS        0x23  //1-byte register
#define EEPROM_TRANSMIT_VOLTAGE     0x24  //1-byte register
#define EEPROM_SENDDELAY_MS         0x25  //2-byte register
#define EEPROM_ALTITUDE             0x27  //2-byte register
#define EEPROM_DAYNIGHT_TYPE        0x29  //1-byte register
#define EEPROM_DAYNIGHT_THRESHOLD   0x30  //3-byte register
#define EEPROM_DAYNIGHT_TX_INTERVAL 0x33  //2-byte register
// nächstes Register ist 0x35 !! 

/**
 * Register indexes
 */
DEFINE_REGINDEX_START()
  REGI_CONFIGURATION,
  REGI_VOLTSUPPLY,
  REGI_ANALOGSENSOR,
  REGI_1WIRE,
  REGI_TEMPHUM,
  REGI_PRESS,
  REGI_LUM,
DEFINE_REGINDEX_END()

#endif

