/**************************************************************************************
Functionality:	
Device talks to Arduino via bluetooth.
Send "a" or "b" from device to arduino. Arduino will give the response.

The circuit:

Arduino 	Bluetooth
 5V			 VCC
 GND		 GND
 TX    		 RX
 RX			 TX


Author:		Jeffrey
Created:	5 October 2014

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include <Arduino.h>
#include <HardwareSerial.h>

const int ledPin =  13;      // the number of the LED pin

void setup() {

	// initialization
	Serial.begin(9600);
	Serial.write("Hello world");
	digitalWrite(ledPin, HIGH);

}
 
 void loop() {

	// Do the things.

	 //Serial.write("Hello world");
	 //delay(1000);

	 if(Serial.available())
	 {
		int c = Serial.read();
		switch(c)
		{
		case 'a':
			Serial.write("Receive a\r\n");
			digitalWrite(ledPin, HIGH);
			break;
		case 'b':
			Serial.write("Receive b\r\n");
			digitalWrite(ledPin, LOW);
			break;
		}
	 }
 }

