/**************************************************************************************
Functionality:	
	<Describe the main functionality of the code in this file.>

The circuit:
	driver	controller
	+5V			+5V
	GND			GND
	PWM1		pin #6
	PWM2		pin #7
	BAT-						battery -
	BAT+						battery +
	OUT1						motor +
	OUT2						motor -

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

#define PWM1 6
#define PWM2 7

int pwm1Duty = 0;
int pwm2Duty = 255;


void setup() {

	// initialization

}
 
 void loop() {

	 analogWrite(PWM1, pwm1Duty);  // analogWrite values from 0 to 255
	 analogWrite(PWM2, pwm2Duty);  // analogWrite values from 0 to 255

 }

