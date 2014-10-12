/*

Demo how to use the ERxIRCmdReceiverService to get the IR signal.

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Open serial monitor 
 Verify there is a line of string "result" received by the monitor each second.

Wiring

Arduino   IR detector sensor
	11			signal line 


* Created 12 October 2014
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxIOStream.h>
#include <ERxOutputStream.h>
#include <ERxHost.h>
#include <ERxService.h>
#include <ERxIRCmdReceiverService.h>
#include <IRremote.h>

// Allocate the buffers
#define COMMAND_PARAMETER_BUFFER_SIZE 128
#define RESULT_BUFFER_SIZE 128
static uint8_t sCmdBuffer[COMMAND_PARAMETER_BUFFER_SIZE];
static uint8_t sResultBuffer[RESULT_BUFFER_SIZE];

// Define the host and the supported services
ERxHost host(sCmdBuffer, COMMAND_PARAMETER_BUFFER_SIZE, sResultBuffer, RESULT_BUFFER_SIZE);

#define IR_INPUT_PIN 11
ERxIRCmdReceiverService irCmdService(IR_INPUT_PIN);

void setup() {
	Serial.begin(9600);
	// Set the address.
	host.SetMyAddress(0x01);
	// Add service to host.
	host.AddService(&irCmdService);
}

void loop() {
	host.Run();
}