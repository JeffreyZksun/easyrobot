/**************************************************************************************
Functionality:	
 Two Arduinos communicate via I2C. Any one cam be the master or slave.

The circuit:
Use atmega 328p and atmega 1280 as test boards.
	ATMega328p				ATMega1280
	SDA(A4)	--------------	SDA(20)
	SCL(A5)	--------------	SCL(21)

	GND		--------------	GND
	+5V		--------------	+5V

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
#include <Wire.h>

#define MASTER_ADDRESS 11
#define SLAVE_ADDRESS 33

#define I2C_MASTER_MODEL
//#define I2C_SLAVE_MODEL

void sendDataFromMasterToSlave()
{
	Serial.println();
	Serial.println("*************sendDataFromMasterToSlave*****************");
	Serial.println();

	Wire.beginTransmission(SLAVE_ADDRESS); // transmit to device #4
	Wire.send("Master");        // sends five bytes
	Wire.endTransmission();    // stop transmitting

	Serial.println();
}

void readDataFromSlave()
{
	Serial.println();
	Serial.println("*************readDataFromSlave*****************");
	Serial.println();
	Wire.requestFrom(SLAVE_ADDRESS, 6);    // request 6 bytes from slave device

	while(Wire.available())    // slave may send less than requested
	{ 
		char c = Wire.receive(); // receive a byte as character
		Serial.print(c);         // print the character
	}
	Serial.println();
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
	Serial.println();
	Serial.println("*************receiveEvent*****************");
	Serial.println();
	while(1 < Wire.available()) // loop through all but the last
	{
		char c = Wire.receive(); // receive byte as a character
		Serial.print(c);         // print the character
	}
	Serial.println();
}


// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent()
{
	Serial.println();
	Serial.println("*************requestEvent*****************");
	Serial.println();
	Wire.send("Slavee"); // respond with message of 6 bytes
	Serial.println();
}

void initMaster()
{
	Wire.begin();        // join i2c bus (address optional for master)
}

void initSlave()
{
	Wire.begin(SLAVE_ADDRESS);        // join i2c bus with address
	Wire.onRequest(requestEvent); // register event
	Wire.onReceive(receiveEvent); // register event
}

void setup() {

#if defined(I2C_MASTER_MODEL)
	initMaster();
#else
	initSlave();
#endif

	Serial.begin(9600);  // start serial for output

}

void loop() {

#if defined(I2C_MASTER_MODEL)
	sendDataFromMasterToSlave(); 
	delay(100);
	readDataFromSlave();
	delay(1000);

#else
	// Respond master in the interruption.
#endif
}
