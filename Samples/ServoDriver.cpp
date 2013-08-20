#include <ERxCore.h>

/**************************************************************************************
Functionality:	
	<Describe the main functionality of the code in this file.>

The circuit:
	A LED is connected to pin 13.
	A servo is connected to pin 9.

More:
	Servo motors have three wires: power, ground, and signal. 
	The power wire is typically red, and should be connected to the 5V pin on the board. 
	The ground wire is typically black or brown and should be connected to a ground pin on the board. 
	The signal pin is typically yellow, orange or white and should be connected to a digital pin on the board. 


Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include "Servo.h"
#include "SerialComHelper.h"
#include "ERxProtocol.h"
SerialComHelper comHepler;
Servo myservo;   // Define the servo
long pos = 0;   
void setup()
{
	// initialize the digital pin as an output:
	pinMode(13, OUTPUT);
	myservo.attach(9, 1000, 2000);				// Connect the servo the pin #9.
	myservo.writeMicroseconds(1000); 

	Serial.begin(9600); 
}

void SendPosition()
{
	long position = pos / 100 ;
	
	unsigned char inst = 0x66;
	
	comHepler.SendInstruction(&Serial, inst, position); 

}
void loop()
{
	for(pos = 1000; pos < 2000; pos += 10)       //From 0 to 180 degree
	{ 
		if(pos > 1500)    
			digitalWrite(13, HIGH);				// set the LED on
		else
			digitalWrite(13, LOW);				// set the LED off

		myservo.writeMicroseconds(pos);         //Change servo degrees
		SendPosition();
		delay(100);                       
	}

	for(pos = 2000; pos>=1000; pos-=10)        //From 180 to 0 degree.
	{        
		if(pos > 1500)    
			digitalWrite(13, HIGH);   
		else
			digitalWrite(13, LOW);   

		myservo.writeMicroseconds(pos);
		SendPosition();
		delay(100);                      
	}
}
