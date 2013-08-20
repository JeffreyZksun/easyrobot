/*

Send message string as a package.

Steps to run this sketch
 1. Connect the Circuit
 2. Build the sketch and upload to Arduino
 3. Open the serial monitor
 Verify message frame is received as a whole.


* Created 2 June 2012
* By Jeffrey Sun
* 

*/
#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxOutputStream.h>
#include <ERxRedirectOutputStream.h>

#define ch 'x'
unsigned char redirectBuffer[64] = {0};
ERxRedirectOutputStream redirectStream(&Serial, redirectBuffer, 64);

void setup() {
	Serial.begin(9600);
}

void loop() {
	redirectStream.print('[');
	for(int i = 0; i < 20; i++)
	{
		redirectStream.print(ch);
		delay(100);
	}
	redirectStream.print("]\r\n");
}