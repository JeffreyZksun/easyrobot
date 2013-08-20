/*

Send the specified the IR code periodly.

Steps to run this sketch
 1. Connect the Circuit
 2. Build the sketch and upload to Arduino
 3. Point the IR transmitter to the IR controlled device.
 Verify the device can receive the IR code.

Circuit:
* Connect IR LED transmitter to the output PWM pin TIMER_PWM_PIN (9) defined in IRremote.h.

* Created 1 June 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <IRremote.h>
#include <ERxIOStream.h>
#include <ERxOutputStream.h>
#include <ERxHost.h>
#include <ERxService.h>
#include <ERxCommandId.h>
#include <ERxCommandGeneratorService.h>
#include <ERxIRControllerService.h>

#define RECV_PIN 11

// Allocate the buffers
#define COMMAND_PARAMETER_BUFFER_SIZE 128
#define RESULT_BUFFER_SIZE 128
static uint8_t sCmdBuffer[COMMAND_PARAMETER_BUFFER_SIZE];
static uint8_t sResultBuffer[RESULT_BUFFER_SIZE];

const char * NECcode = "0xFF827D"; // back

// Define the host and the supported services
ERxHost host(sCmdBuffer, COMMAND_PARAMETER_BUFFER_SIZE, sResultBuffer, RESULT_BUFFER_SIZE);
ERxCommandGeneratorService cmdGenService(IR_SEND_NEC, NECcode, &Serial);
ERxIRControllerService irService(RECV_PIN);

void setup() {

	Serial.begin(9600);

	// Set the address.
	host.SetMyAddress(0x01);

	// Add service to host.
	host.AddService(&cmdGenService);
	host.AddService(&irService);
}

void loop() {

	host.Run();

	delay(2000);
}