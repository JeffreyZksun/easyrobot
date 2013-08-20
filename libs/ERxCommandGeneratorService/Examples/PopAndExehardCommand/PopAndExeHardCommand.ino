/*

Demo how to use the ERxCommandGeneratorService and ERxCommandExecutorService to populate and execute hard command.

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Open serial monitor 
 Verify there is a line of string "result" received by the monitor each second.


* Created 3 June 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxIOStream.h>
#include <ERxOutputStream.h>
#include <ERxHost.h>
#include <ERxService.h>
#include <ERxCommandId.h>
#include <ERxCommandGeneratorService.h>
#include <ERxCommandExecutorService.h>

// Allocate the buffers
#define COMMAND_PARAMETER_BUFFER_SIZE 128
#define RESULT_BUFFER_SIZE 128
static uint8_t sCmdBuffer[COMMAND_PARAMETER_BUFFER_SIZE];
static uint8_t sResultBuffer[RESULT_BUFFER_SIZE];


// Define the host and the supported services
ERxHost host(sCmdBuffer, COMMAND_PARAMETER_BUFFER_SIZE, sResultBuffer, RESULT_BUFFER_SIZE);
ERxCommandGeneratorService cmdGenService(12, "para\r\n", &Serial);
ERxCommandExecutorService cmdExeService(12, "result\r\n");

void setup() {

	Serial.begin(9600);

	// Set the address.
	host.SetMyAddress(0x01);

	// Add service to host.
	host.AddService(&cmdGenService);
	host.AddService(&cmdExeService);
}

void loop() {

	host.Run();

	delay(1000);
}