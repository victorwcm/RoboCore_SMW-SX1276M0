/*******************************************************************************
* SMW_SX1276M0 Simple Send with CayenneLPP - OTAA (v1.0)
* 
* Simple program to test sending a message with OTAA and CayenneLPP encoding. 
*
* This program uses the ESP32 to communicate with the LoRaWAN module.
*
* Written by Victor Medeiros (16/03/2023)
* 
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

#if !defined(ARDUINO_ESP32_DEV) // ESP32
#error Use this example with the ESP32
#endif

// --------------------------------------------------
// Libraries

#include "RoboCore_SMW_SX1276M0.h"
#include <CayenneLPP.h>

// --------------------------------------------------
// Variables

#include <HardwareSerial.h>
HardwareSerial LoRaSerial(2);
#define RXD2 16
#define TXD2 17

SMW_SX1276M0 lorawan(LoRaSerial);
CayenneLPP lpp(50);

CommandResponse response;

const char APPEUI[] = "0000000000000000";
const char APPKEY[] = "00000000000000000000000000000000";

const unsigned long PAUSE_TIME = 300000; // [ms] (5 min)
unsigned long timeout;

// --------------------------------------------------
// Prototypes

void event_handler(Event);

// --------------------------------------------------
// --------------------------------------------------

void setup() {
  // Start the UART for debugging
  Serial.begin(115200);
  Serial.println(F("--- SMW_SX1276M0 Uplink (OTAA) ---"));
  
  // start the UART for the LoRaWAN Bee
  LoRaSerial.begin(115200, SERIAL_8N1, RXD2, TXD2);

  // set the event handler
  lorawan.event_listener = &event_handler;
  Serial.println(F("Handler set"));

  lorawan.setPinReset(5);
  lorawan.reset();

  // read the Device EUI
  char deveui[16];
  response = lorawan.get_DevEUI(deveui);
  if(response == CommandResponse::OK){
    Serial.print(F("DevEUI: "));
    Serial.write((uint8_t *)deveui, 16);
    Serial.println();
  } else {
    Serial.println(F("Error getting the Device EUI"));
  }

  // set the Application EUI
  response = lorawan.set_AppEUI(APPEUI);
  if(response == CommandResponse::OK){
    Serial.print(F("Application EUI set ("));
    Serial.write((uint8_t *)APPEUI, 16);
    Serial.println(')');
  } else {
    Serial.println(F("Error setting the Application EUI"));
  }

  // set the Application Key
  response = lorawan.set_AppKey(APPKEY);
  if(response == CommandResponse::OK){
    Serial.print(F("Application Key set ("));
    Serial.write((uint8_t *)APPKEY, 32);
    Serial.println(')');
  } else {
    Serial.println(F("Error setting the Application Key"));
  }

  // set join mode to OTAA
  response = lorawan.set_JoinMode(SMW_SX1276M0_JOIN_MODE_OTAA);
  if(response == CommandResponse::OK){
    Serial.println(F("Mode set to OTAA"));
  } else {
    Serial.println(F("Error setting the join mode"));
  }

  // join the network
  Serial.println(F("Joining the network"));
  lorawan.join();
}

// --------------------------------------------------
// --------------------------------------------------

void loop() {
  // listen for incoming data from the module
  lorawan.listen();

  // send a message
  if(lorawan.isConnected()){
    if(timeout < millis()){
      printVariables();
      // send the message
      String data = convertToHex(lpp.getBuffer(), lpp.getSize());
      Serial.print("Sending: "); Serial.println(data);
      response = lorawan.sendX(1, data);
  
      // update the timeout
      timeout = millis() + PAUSE_TIME;
    }
  } else {
    if(timeout < millis()){
      // show some activity
      Serial.println('.');
    
      // update the timeout
      timeout = millis() + 5000; // 5 s
    }
  }
}

// --------------------------------------------------
// --------------------------------------------------

// Handle the events of the module
//  @param (type) : the type of the event [Event]
void event_handler(Event type){
  // check if join event
  if(type == Event::JOINED){
    Serial.println(F("Joined"));
  }
}

void printVariables()
{
  lpp.reset();

  int humidity = random(0,100);
  lpp.addRelativeHumidity(1, humidity);

  int temp = random(0,100);
  lpp.addTemperature(1, temp);

  int lux = random(0,100);
  lpp.addLuminosity(1, lux);
}

String convertToHex(uint8_t *buffer, uint8_t size) {
  String ret = "";
  char temp_char = '0';
  uint8_t cur_value = 0;
  for(int i = 0; i < size; i++) {
    cur_value = buffer[i];
    temp_char = '0';
    temp_char += (cur_value / 16);
    if(temp_char > '9'){
        temp_char += 7;
    }
    ret.concat(temp_char);
    temp_char = '0';
    temp_char += (cur_value % 16);
    if(temp_char > '9'){
        temp_char += 7;
    }
    ret.concat(temp_char);
  }
  return ret;
}

// --------------------------------------------------