#include <ERxCore.h>

/**************************************************************************************
Functionality:	
	Turns on and off a light emitting diode(LED) connected to digital  
	pin 13, when pressing a bumper sensor attached to pin 21. 
	Also the status is send out through the serial port 0.

The circuit:
	 LED attached from pin 13 to ground.
	 A bumper sensor is connected to pin 21.

More:
	The interrupt number passed to attachInterrupt isn't the number defined in ATMEGA.
	By analyzing the function, we can get the following relationship.
	
	External Interrupts: 	
	attachInterrupt param #		Arduino pin #		ATMEGA pin #
			0						2					INT4
			1						3					INT5
			2						21					INT0
			3						20					INT1
			4						19					INT2
			5						18					INT3
			6						N/A					INT6
			7						N/A					INT7
			
Occupied MCU resource:
	The external interrupt pins.

Author:	Jeffrey Sun
Created: 7/2010

**************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
const int ledPin =  13;      // the number of the LED pin

volatile int state = LOW;

void buttonClick()
{
	state = !state;
  
	Serial.println(state);
}

void setup()
{
	Serial.begin(9600); 
	pinMode(ledPin, OUTPUT);
	attachInterrupt(2, buttonClick, FALLING);
}

void loop()
{
	digitalWrite(ledPin, state);
}


//===========================================================================
