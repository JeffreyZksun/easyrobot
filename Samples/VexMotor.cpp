/**************************************************************************************
Functionality:	
Arduino controls VEX 3-line motor via PWM.
Once power Arduino, the motor will repeat four stages, which are forward, stop, backward stop.
Each stage lasts for 2 seconds.

In VEX, the range of values for speed control is between 0 and 255, with 127 as center. 
In Arduino, typically the on time of the PWM pulse will be varied from around 1 millisecond to 2 milliseconds
 with the off time around 20 milliseconds.
To control VEX motor, the range of PWM signal for the speed control is 1000 us to 2000 us., with 1500 as center.

The circuit:

Arduino----VEX
 +5V--------Red line
 GND--------Black line
 6----------White line

Dependency
 Servo

Occupied MCU resource:
PWM, timer

Open question
[1] I haven't tried the effect on the speed when set the value less than 1000 us or more than 2000 us.

Reference: 
[1] wiring, http://www.robotc.net/wiki/Tutorials/Arduino_Projects/Mobile_Robotics/VEX/Connecting_A_VEX_Motor
[2] Servo library, http://arduino.cc/en/reference/servo
[3] PWM frequency, http://arduino.cc/en/Tutorial/PWM

Author:		Jeffrey Sun
Created:	7 October 2014

***************************************************************************************/
#include <Arduino.h>
#include <HardwareSerial.h>
#include <Servo.h> 

#define MOTOR_PIN 6
// define the PWM range for the VEX speed control
#define MIN_POS 1000 // 0
#define MAX_POS 2000 // 255
#define CENTER_POS 1500 // 127

Servo myservo;
int status = 0;

void setup() {
	Serial.begin(9600);
	Serial.println("initializing...");
	myservo.attach(MOTOR_PIN);
}
void loop() {
	int c = status % 4;
	if(0 == c){
		myservo.writeMicroseconds(MIN_POS);
		Serial.println("forward");
	}
	else if(2 == c){
		myservo.writeMicroseconds(MAX_POS);
		Serial.println("backward");
	}
	else{
		myservo.writeMicroseconds(CENTER_POS);
		Serial.println("stop");
	}
	status++;
	delay(2000);
}