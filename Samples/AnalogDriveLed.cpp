/**************************************************************************************
Functionality:	
	Read the analog value and send out by serial port 0.

The circuit:
	Connect a line tracker sensor to ANALOG pin 0;

More:

Occupied MCU resource:
	ADC pins.

Author:	Jeffrey Sun
Created: 7/2010

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxCore.h>
#include "SerialComHelper.h"
#include "ERxProtocol.h"
SerialComHelper comHepler;

void setup() {
  Serial.begin(9600); 
 }
 
 void loop() {
	// read the analog input into a variable:
   long analogValue = analogRead(0);
   // print the result:

   unsigned char inst = 0x01;
   comHepler.SendInstruction(&Serial, inst, analogValue);

   delay(500);
 }
