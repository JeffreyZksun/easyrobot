/**************************************************************************************
Functionality:	
	<Describe the main functionality of the code in this file.>

The circuit:
	<Describe the used circuit connection.>

More:
	<Add more information regarding the hardware>

Occupied MCU resource:
	<Indicate which resources are occupied by this functionality. 
	When try to implement other functionalities, these resources shouldn't be used again.>

Author:		<Author>
Created:	<Date>

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

