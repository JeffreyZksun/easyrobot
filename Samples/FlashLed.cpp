/**************************************************************************************
Functionality:	
	The LED on board flashes automatically.

The circuit:
	LED attached from pin 13 to ground.

More:
	<Add more information regarding the hardware>

Occupied MCU resource:
	<Indicate which resources are occupied by this functionality. 
	When try to implement other functionalities, these resouces shouldn't be used again.>

Author:		Jeffrey Sun
Created:	7/2010

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============

#include <Arduino.h>

const int ledPin =  13;      // the number of the LED pin
boolean bIsLedOpen = 1;

void setup() {

	// initialization
	pinMode(ledPin, OUTPUT);
}
 
 void loop() {

	// Do the things.
	 bIsLedOpen = !bIsLedOpen;
	 digitalWrite(ledPin, bIsLedOpen);

	 delay(500);
 }
