/**************************************************************************************
Functionality:	
Connect the serial port 0 and serial port 1.
Receive data from serial 0 and send to serial 1.
Receive data from serial 1 and sent to serial 0.

The circuit:
Install the xbee model to serial 0.
Connect the USB to PC.
Connect the bluetooth serial port mode to serial 1.

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

void setup() {

	// initialization
	Serial.begin(9600);
	Serial1.begin(9600);

}

void RedirectData(HardwareSerial* pSrc, HardwareSerial* pDest)
{
	if(NULL == pSrc || NULL == pDest)
		return;

	if(pSrc->available())
	{
		uint8_t c = pSrc->read();
		pDest->write(c);
	}

}

void loop() {

	// Do the things.
	RedirectData(&Serial, &Serial1);
	RedirectData(&Serial1, &Serial);
}
