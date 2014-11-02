/*
Drive the DC motor with the ErxL298N library. 
The motor will run forward for 2 seconds, then stop for 2 seconds,
then run backward for 2 seconds, then stop for 2 seconds. 
Then begin the next loop.

Aurdino pin #4 and #5 connects to E1 and M1 of L298N respectively.
L298N M1 connects to a DC motor. In this example, only one motor is used. 

The circuit:
---------------------------------------------
	Arduino		L298N(Motor Driver)			
	4				E1
	5				M1

	GND				GND
	VIN(9V)			VD

---------------------------------------------
	Motor Battery	   L298N	 Ardiono
	GND					GND		  GND
	+4.8~35V			VS		  N/A

---------------------------------------------
	Logic Battery	   L298N	 Ardiono
	GND					GND		  GND
	+7~12V				VD	  	  Vin

---------------------------------------------
	L298N		Motor
	M1			DC Motor

* Created 4 October 2014
* By Jeffrey Sun

*/

#include <Arduino.h>
#include <ERxIMotor.h>
#include <ERxL298N.h>
#include <HardwareSerial.h>

#define E1 4
#define M1 5

ERxL298N motor(E1, M1);
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
