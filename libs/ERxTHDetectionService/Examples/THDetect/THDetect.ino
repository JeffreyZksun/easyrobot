/*

Detect the temperature and humidity via am2311 and dht11 each minute.

Steps to run this sketch
 1. Connect the Circuit
 2. Build the sketch and upload to Arduino
 3. Open serial monitor 

Circuit:
* Connect the data pin of DHT11 to Arduino pin 2 (DHT11PIN)
* Connect the SDA and SCL pins of AM2311 to SDA and SCL pins of Arduino
* Connect the VCC and GND pins of DHT11 and AM2311 to +5 and GND pins of Arduino

* Created 1 June 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>
#include <DHT11.h>
#include <ERxAM2311.h>
#include <ERxIOStream.h>
#include <ERxOutputStream.h>
#include <ERxHost.h>
#include <ERxService.h>
#include <ERxCommandId.h>
#include <ERxCommandGeneratorService.h>
#include <ERxTHDetectionService.h>

#define DHT11PIN 2

// Allocate the buffers
#define COMMAND_PARAMETER_BUFFER_SIZE 128
#define RESULT_BUFFER_SIZE 128
static uint8_t sCmdBuffer[COMMAND_PARAMETER_BUFFER_SIZE];
static uint8_t sResultBuffer[RESULT_BUFFER_SIZE];


// Define the host and the supported services
ERxHost host(sCmdBuffer, COMMAND_PARAMETER_BUFFER_SIZE, sResultBuffer, RESULT_BUFFER_SIZE);
ERxCommandGeneratorService cmdGenService(TH_GET, NULL, &Serial);
ERxTHDetectionService thDetectionService(DHT11PIN);

void setup() {

	Serial.begin(9600);

	// Set the address.
	host.SetMyAddress(0x01);

	// Add service to host.
	host.AddService(&cmdGenService);
	host.AddService(&thDetectionService);
}

void loop() {

	host.Run();

	delay(1000);
}