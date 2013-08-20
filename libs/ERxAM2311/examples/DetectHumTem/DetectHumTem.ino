/*
Detect the humidity and temperature with AOSONG AM2311.
Demonstrates use of the ERxAM2311 library. 

The circuit:
 * Connect the SDA and CSL pins between sensor AM2311 and Arduino.

Duemilanove: A4 (SDA) and A5 (SCL)
	http://arduino.cc/en/Main/ArduinoBoardDuemilanove
Mega: 20 (SDA) and 21 (SCL).
	http://arduino.cc/en/Main/ArduinoBoardMega

AM2311: YELLOW (SDA) and WHITE (SCL)
	http://www.aosong.com/cn/products/details.asp?id=93
	http://item.taobao.com/item.htm?id=14616712897


* Created 18 December 2011
* By Jeffrey Sun

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxAM2311.h>
#include <Wire.h>

int iLoopIndex = 0;

ERxAM2311 amSensor;

void setup() {

	Serial.begin(9600);
	Serial.println("Starting...");
}

void loop() {

	Serial.print("Detect temperature and humidity => ");
	Serial.println(++iLoopIndex);

	int ret = amSensor.read();
	Serial.print("Read result: ");
	Serial.println(ret);

	Serial.print("Humidity (%RH): ");
	Serial.println(amSensor.humidity, 2);
	Serial.print("Temperature (C): ");
	Serial.println(amSensor.temperature, 2);
	Serial.println();

	delay(2000);
}
