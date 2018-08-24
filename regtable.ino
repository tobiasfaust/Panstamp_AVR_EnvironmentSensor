/**
 * regtable
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
 * Creation date: 03/31/2011
 *
 * Development Steps:
 * https://code.google.com/p/panstamp/wiki/developmentsteps
 */

#include <EEPROM.h>
#include "product.h"
#include "panstamp.h"
#include "regtable.h"



/**
 * Declaration of common callback functions
 */
DECLARE_COMMON_CALLBACKS()

/**
 * Definition of common registers
 */
DEFINE_COMMON_REGISTERS()

/*
 * Definition of custom registers, see at EnvironmentSensor.xml
 */

REGISTER regConfiguration(dtConfiguration,
                            sizeof(dtConfiguration),
                            &GetConfiguration,
                            &SetConfiguration);


static unsigned long voltageSupply = 3300;
REGISTER regVoltSupply(dtVoltSupply,                 // Pointer to the data container
                       sizeof(dtVoltSupply),         // Size of the data container
                       &updtVoltSupply,              // Pointer to the updater function
                       NULL);                        // Pointer to the setter function


REGISTER regAnalogSensor(dtAnalogSensor, 
                               sizeof(dtAnalogSensor), 
                               &updtAnalogSensor, 
                               NULL);

REGISTER reg1WireSensor(dt1WireSensor, 
                              sizeof(dt1WireSensor), 
                              &updt1WireSensor, 
                              NULL);
                              
REGISTER regTempHumSensor(dtTempHumSensor, 
                              sizeof(dtTempHumSensor), 
                              &updtTempHumSensor, 
                              NULL);

REGISTER regPressSensor(dtPressSensor, 
                              sizeof(dtPressSensor), 
                              &updtPressSensor, 
                              NULL);
                              
REGISTER regLumSensor(dtLumSensor, 
                              sizeof(dtLumSensor), 
                              &updtLumSensor, 
                              NULL);

/**
 * Initialize table of registers
 */
DECLARE_REGISTERS_START()
  &regConfiguration,                 // Register ID = 11 (0x0B)
  &regVoltSupply,                    // Register ID = 12 (0x0C)
  &regAnalogSensor,                  // Register ID = 13 (0x0D)
  &reg1WireSensor,                   // Register ID = 14 (0x0E)
  &regTempHumSensor,                 // Register ID = 15 (0x0F)
  &regPressSensor,                    // Register ID = 16 (0x10)
  &regLumSensor                      // Register ID = 17 (0x11)
DECLARE_REGISTERS_END()

/**
 * Definition of common getter/setter callback functions
 */
DEFINE_COMMON_CALLBACKS()

/**
 * Definition of custom getter/setter callback functions
 */
 
/**
 * updtVoltSupply
 *
 * Measure voltage supply and update register
 *
 * 'rId'  Register ID
 */
const void updtVoltSupply(byte rId)
{  
  unsigned long result;
  
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  voltageSupply = result;     // Update global variable Vcc
  unsigned short ref = voltageSupply;
  
  if(TX_BATVOLT){
    // Read voltage supply from A7
    result = analogRead(VOLT_SUPPLY);
    result *= ref;
    result /= 1024;
    #ifdef DEBUG
      Serial.print("Wert Batterie: ");Serial.println(result, HEX);
    #endif
    // Update register value
    /** register[eId]->member can be replaced by regVoltSupply in this case since
    *  no other register is going to use "updtVoltSupply" as "updater" function  */
    dtVoltSupply[0] = (result >> 8) & 0xFF;    // Replaceable by: regTable[rId]->value[0] = (result >> 8) & 0xFF;
    dtVoltSupply[1] = result & 0xFF;           // Replaceable by: regTable[rId]->value[1] = result & 0xFF;
  }
  
  if(TX_SOLVOLT) {
    // Read voltage supply from A7
    result = analogRead(SOLAR_SUPPLY);
    result *= ref;
    result /= 1024;
    #ifdef DEBUG
      Serial.print("Wert Solar: ");Serial.println(result, HEX);    
    #endif
    dtVoltSupply[2] = (result >> 8) & 0xFF;
    dtVoltSupply[3] = result & 0xFF;
  }

}

/**
 * updtSensor
 *
 * Measure sensor data and update register
 *
 * 'rId'  Register ID
 */
const void updtAnalogSensor(byte rId)
{
  sensor_ReadAnalog();
}


/**
 * updt1WireSensor
 *
 * Measure sensor data and update register
 *
 * 'rId'  Register ID
 */
const void updt1WireSensor(byte rId)
{  
  sensor_ReadOneWireTemp();
}

/**
 * updtTempHumSensor
 *
 * Measure sensor data and update register
 *
 * 'rId'  Register ID
 */
const void updtTempHumSensor(byte rId)
{
  //sensor_ReadOneWireTemp();
  sensor_ReadDHT();
}


/**
 * updtPressSensor
 *
 * Measure sensor data and update register
 *
 * 'rId'  Register ID
 */
const void updtPressSensor(byte rId)
{
  sensor_ReadBMP180();
}

/**
 * updtLumSensor
 *
 * Measure sensor data and update register
 *
 * 'rId'  Register ID
 */
const void updtLumSensor(byte rId)
{
  sensor_ReadTSL2561();
}

/**
 * setConfiguration
 *
 * Set Confuguration and store in EEPROM
 *
 * 'rId'         Register ID
 * 'value'       New value
 */
const void SetConfiguration(byte rId, byte *value) {
  // Update register
  if ((value[0] != regTable[rId]->value[0]) ||            \
      (value[1] != regTable[rId]->value[1]) ||            \
      (value[2] != regTable[rId]->value[2]) ||            \
      (value[3] != regTable[rId]->value[3]) ||            \
      (value[4] != regTable[rId]->value[4]) ||            \
      (value[5] != regTable[rId]->value[5]) ||            \
      (value[6] != regTable[rId]->value[6]) ||            \
      (value[7] != regTable[rId]->value[7]) ||            \
      (value[8] != regTable[rId]->value[8]) ||            \
      (value[9] != regTable[rId]->value[9]) ||            \
      (value[10] != regTable[rId]->value[10]) ||            \
      (value[11] != regTable[rId]->value[11]) ||            \
      (value[12] != regTable[rId]->value[12]) ||            \
      (value[13] != regTable[rId]->value[13]) ||            \
      (value[14] != regTable[rId]->value[14]))              \
  {
    #ifdef DEBUG
       Serial.println("Store Configuration");
    #endif
    memcpy(regTable[rId]->value, value, sizeof(regTable[rId]->value));
    EEPROM.write(EEPROM_SYNC_INTERVAL,          value[0]);
    EEPROM.write(EEPROM_SYNC_INTERVAL + 1,      value[1]);

    EEPROM.write(EEPROM_SENDDELAY_MS,           value[2]);
    EEPROM.write(EEPROM_SENDDELAY_MS + 1,       value[3]);

    EEPROM.write(EEPROM_ALTITUDE,               value[4]);
    EEPROM.write(EEPROM_ALTITUDE + 1,           value[5]);
    
    EEPROM.write(EEPROM_DAYNIGHT_TYPE,          value[6]);
    EEPROM.write(EEPROM_DAYNIGHT_THRESHOLD,     value[7]);    
    EEPROM.write(EEPROM_DAYNIGHT_THRESHOLD+1,   value[8]);
    EEPROM.write(EEPROM_DAYNIGHT_THRESHOLD+2,   value[9]);
    EEPROM.write(EEPROM_DAYNIGHT_TX_INTERVAL,   value[10]);
    EEPROM.write(EEPROM_DAYNIGHT_TX_INTERVAL+1, value[11]);    
    
    EEPROM.write(EEPROM_USING_ANALOG,           value[12]);
    EEPROM.write(EEPROM_USING_SENSORS,          value[13]);
    EEPROM.write(EEPROM_TRANSMIT_VOLTAGE,       value[14]);
        
    // neue Parameter in die Variablen laden
    getVarsFromEeprom();
  }
}

/**
 * GetConfiguration
 *
 * Collect ConfigurationParameters in Register
 *
 * 'rId'         Register ID
 * 'value'       New value
 */
const void GetConfiguration(byte rId) {
  // Update register
  dtConfiguration[0] = SyncInterval[0];
  dtConfiguration[1] = SyncInterval[1];
  dtConfiguration[2] = SENDDELAY_MS[0]; 
  dtConfiguration[3] = SENDDELAY_MS[1];
  dtConfiguration[4] = ALTITUDE[0]; 
  dtConfiguration[5] = ALTITUDE[1];
  dtConfiguration[6] = DAYNIGHT_TYPE;
  dtConfiguration[7] = DAYNIGHT_THRESHOLD[0];
  dtConfiguration[8] = DAYNIGHT_THRESHOLD[1];
  dtConfiguration[9] = DAYNIGHT_THRESHOLD[2];
  dtConfiguration[10] = DAYNIGHT_TX_INTERVAL[0];
  dtConfiguration[11] = DAYNIGHT_TX_INTERVAL[1];
  dtConfiguration[12] = (HAS_ANALOG4 << 3)          | (HAS_ANALOG3 << 2) | (HAS_ANALOG2 << 1) | (HAS_ANALOG1);
  dtConfiguration[13] = (HAS_BMP180  << 3)          | (HAS_TSL2561 << 2) | (HAS_DHT22   << 1) | (HAS_DS18B20);
  dtConfiguration[14] = (ENABLE_DAYNIGHTSHIFT << 3) | (ENABLE_LED  << 2) | (TX_SOLVOLT  << 1) | (TX_BATVOLT);
}
