/*
Drive the DC motor with the ERxL9110 library. 
The motor will run forward for 2 seconds, then stop for 2 seconds,
then run backward for 2 seconds, then stop for 2 seconds. 
Then begin the next loop.

Aurdino pin #4 and #5 connects to IA1 and IB1 of L9110 respectively.
L9110 OA1, OB1 connects to a DC motor. In this example, only one motor is used. 

The circuit:
---------------------------------------------
	Arduino		L9110(Motor Driver)			
	4				A1
	5				B1

	GND				GND

---------------------------------------------
	Motor Battery	   L9110	 Ardiono
	GND					GND		  GND
	+4.8~35V			VCC		  N/A

---------------------------------------------
	Logic Battery	 	 Ardiono
	GND					  GND
	+7~12V			  	  Vin

---------------------------------------------
	L9110				  Motor
	OA1, OB1			DC Motor

* Created 2 November 2014
* By Jeffrey Sun

*/

#include <Arduino.h>
#include <ERxIMotor.h>
#include <ERxL9110.h>
#include <HardwareSerial.h>

#define IA1 4
#define IB1 5

ERxL9110 motor(IA1, IB1);
int status = 0;

void setup() {
	Serial.begin(9600);
	Serial.println("initializing...");
}

void loop() {

	int c = status % 4;
	if(0 == c){
		motor.forward();
		Serial.println("forward");
	}
	else if(2 == c){
		motor.backward();
		Serial.println("backward");
	}
	else{
		Serial.println("stop");
		motor.stop();
	}
	status++;

	delay(2000);
}
