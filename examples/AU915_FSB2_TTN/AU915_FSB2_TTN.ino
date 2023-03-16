/*******************************************************************************
* SMW_SX1276M0 AU915 FSB2 TTN (v1.0)
* 
* Simple program to configure the module to work with the 
* Australia 915-928 MHz, FSB 2 (used by TTN) frequency plan.
* 
* Written by Victor Medeiros (15/03/23).
* 
* This file is part of a fork of the SMW_SX1276M0 library ("SMW_SX1276M0-lib").
* 
* "SMW_SX1276M0-lib" is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* "SMW_SX1276M0-lib" is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
* 
* You should have received a copy of the GNU Lesser General Public License
* along with "SMW_SX1276M0-lib". If not, see <https://www.gnu.org/licenses/>
*******************************************************************************/

// TTN Frequency Plan for Brazil - 
// AU915-928
// National Telecommunications Agency (ANATEL) Resolution No. 680, from June 27, 2017 - Portuguese only, Article 10
// National Telecommunications Agency (ANATEL) Act No. 14448, from December 4, 2017 - Portuguese only Section 10.3
// Uplink:

// 916.8 - SF7BW125 to SF12BW125
// 917.0 - SF7BW125 to SF12BW125
// 917.2 - SF7BW125 to SF12BW125
// 917.4 - SF7BW125 to SF12BW125
// 917.6 - SF7BW125 to SF12BW125
// 917.8 - SF7BW125 to SF12BW125
// 918.0 - SF7BW125 to SF12BW125
// 918.2 - SF7BW125 to SF12BW125
// 917.5 - SF8BW500
// Downlink:

// 923.3 - SF7BW500 to SF12BW500 (RX1)
// 923.9 - SF7BW500 to SF12BW500 (RX1)
// 924.5 - SF7BW500 to SF12BW500 (RX1)
// 925.1 - SF7BW500 to SF12BW500 (RX1)
// 925.7 - SF7BW500 to SF12BW500 (RX1)
// 926.3 - SF7BW500 to SF12BW500 (RX1)
// 926.9 - SF7BW500 to SF12BW500 (RX1)
// 927.5 - SF7BW500 to SF12BW500 (RX1)
// 923.3 - SF12BW500 (RX2)


#if !defined(ARDUINO_ESP32_DEV) // ESP32
#error Use this example with the ESP32
#endif

// --------------------------------------------------
// Libraries
#include "RoboCore_SMW_SX1276M0.h"

// --------------------------------------------------
// Variables

#include <HardwareSerial.h>
HardwareSerial LoRaSerial(2);
#define RXD2 16
#define TXD2 17

SMW_SX1276M0 lorawan(LoRaSerial);

CommandResponse response;

void setup() {
  // start the UART for debugging
  Serial.begin(115200);
  Serial.println(F("--- SMW_SX1276M0 AU915-928 FSB 2 Frequency Plan Configuration ---"));
  
  // start the UART for the LoRaWAN Bee
  LoRaSerial.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // defines the module reset pin
  lorawan.setPinReset(5);
  // resets the module
  lorawan.reset(); 

  // enables FSB2 channels and disable non-FSB 2 channels
  for(int i = 0; i < 72; i++) {
    // enable FSB 2 frquencies
    if((i >= 8 && i <= 15) || i == 65) {
      response = lorawan.set_CH(i, true);
    }
    // disable non-FSB2 frequencies
    else {
      response = lorawan.set_CH(i, false);
    }
    if(response == CommandResponse::OK){
      Serial.print(F("change channel ")); Serial.print(i); Serial.println(" status - OK");
    }
    else {
      Serial.print(F("change channel ")); Serial.print(i); Serial.println(" status - FAILED");
    }
  }
    Serial.println(F("--- FINISHED - SMW_SX1276M0 AU915-928 FSB 2 Frequency Plan Configuration ---"));
}

void loop() {
}
