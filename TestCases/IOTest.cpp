#include <ERxCore.h>

/**************************************************************************************
Functionality:	
	<Describe the main functionality of the code in this file.>

The circuit:
	<Describe the used circuit connection.>

More:
	<Add more information regarding the hardware>

Occupied MCU resource:
	<Indicate which resources are occupied by this functionality. 
	When try to implement other functionalities, these resouces shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include "ERxProtocol.h"
#include "SerialComHelper.h"

SerialComHelper comHepler;

void setup() {

	// initialization
	Serial.begin(9600);

	DDRA = 0x00; //output
	PORTA = 0xFF; 
	PINA = 0x00; // toggle the port a
	//PUD = 1;

	comHepler.SendInstruction(&Serial, 0x00, PORTA);
}

void TestADPinsOnArduinoBoard()
{
	// Use the AD pins as digital pins.
	// Verify the PF0 pin flips.
	// Based on the pin definition in pins_arduino.c
	// The index of analog pin begins with 54
	uint8_t ADPinIndex = 2; // PF2
	uint8_t digitalPinIndexInArdunioBoard = ADPinIndex + 54;

	// set as digital output
	pinMode(digitalPinIndexInArdunioBoard, OUTPUT);

	digitalWrite(digitalPinIndexInArdunioBoard, HIGH);
	delay(2000);
	digitalWrite(digitalPinIndexInArdunioBoard, LOW);
	delay(2000);

}
 void loop() {

	// Do the things.
	 TestADPinsOnArduinoBoard();
 }
