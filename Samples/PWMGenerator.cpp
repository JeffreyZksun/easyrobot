#include <ERxCore.h>

/**************************************************************************************
Functionality:	
	Reads an analog input pin, maps the result to a range from 0 to 255
	and uses the result to set the pulsewidth modulation (PWM) of an output pin.
	Also prints the results to the serial monitor.

The circuit:
	A LED is connected to pin 13.
	A Line tracker is connected to AD pin 0.

More:


Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include "SerialComHelper.h"
#include "ERxProtocol.h"
SerialComHelper comHepler;

int ledPin = 13;      // LED connected to digital pin 13
int analogPin = 0;   // Line tracker connected to analog pin 0


void setup() {
	Serial.begin(9600); 
}

void loop() {
	// read the analog input into a variable:
	long analogValue = analogRead(analogPin);

	analogWrite(ledPin, analogValue / 4);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255

	// print the result:
	comHepler.SendInstruction(&Serial, 1,analogValue);
	
	delay(500);
}
