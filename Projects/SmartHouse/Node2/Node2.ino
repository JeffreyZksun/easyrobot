/*
Features
 * Smart house: Node 2. Address: 0x02
 * This node is an end device 
 * It receives commands from the external devices, dispatches to the correponding service, 
    and returns the result to the external device where the command is from.

Steps to deploy this sketch
 1. Configure the sketch as the Configuration section.
 2. Build and upload the sketch to aruino board.
 3. Wire the harwares as the Wire section.
 4. Power up the arduino board.

Test local commands
 * Help: "0x50 0x02 0xFA 0x01\r\n"
 * Detect TH: "0x50 0x02 0xFA 0x40\r\n"
 * Send IR "0x50 0x02 0xFA 0x20 0xFF827D\r\n"

Configuration
 * Set MY_ADDRESS. Make sure it is unique in the smart house network. Use a different address if necessary.
 * Set ROUTER_RELAY_ENABLE. If the Ardunio connects to XBee coordinator, set it true. Otherwise, set it false.
 * Set RECV_PIN to use a differen one if necessary. (Any pin is OK)
 * Set DHT11PIN to use a differen one if necessary. (Any pin is OK)

Hardwares
    Duemilanove 328p
    xBee (S1, end)
    Temperature & Humidity sensors (2)
    IR receiver and sender 
Services
    System
    UARTCmdReceiver
    MessageRouter
    TH detection
    IR controller

Circuit:
* Connect IR LED transmitter to the output PWM pin TIMER_PWM_PIN (9) defined in IRremote.h.
* Connect IR LED receiver to the pin RECV_PIN.
* Connect the data pin of DHT11 to Arduino pin 2 (DHT11PIN)
* Connect the SDA and SCL pins of AM2311 to SDA and SCL pins of Arduino
* Connect the VCC and GND pins of DHT11 and AM2311 to +5 and GND pins of Arduino
* Stack the XBee end device

Requried library
	IRremote
	DHT11
	ERxXxxx : http://code.google.com/p/easyrobot

* Created 3 June 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <IRremote.h>
#include <Wire.h>
#include <DHT11.h>
#include <ERxAM2311.h>
#include <ERxIOStream.h>
#include <ERxOutputStream.h>
#include <ERxTextMessage.h>
#include <ERxHost.h>
#include <ERxService.h>
#include <ERxSystemService.h>
#include <ERxUARTCmdReceiverService.h>
#include <ERxMessageRouterService.h>
#include <ERxIRControllerService.h>
#include <ERxTHDetectionService.h>

//--------------CONFIGURATION-------------------------------------------------------------
// Macro definitions which require modifying for different harwares.
//
#define MY_ADDRESS 0x02
// ***IMPORTANT*** :Set true on the coordinator, and false on end device.
#define ROUTER_RELAY_ENABLE false

#define RECV_PIN 11

#define DHT11PIN 2
//----------------------------------------------------------------------------------------
// Allocate the buffers
#define COMMAND_PARAMETER_BUFFER_SIZE 128
#define RESULT_BUFFER_SIZE 128
static uint8_t sCmdBuffer[COMMAND_PARAMETER_BUFFER_SIZE];
static uint8_t sResultBuffer[RESULT_BUFFER_SIZE];

// XBee
// AES encryption dictates that the maximum RF packet size is 95 Bytes.
// The XBee buffer size is 95. To make sure the message sent to XBee is packed into a package, this buffer should be less than 95.
#define ROUTER_BUFFER_SIZE 90
static uint8_t sRouterBuffer[ROUTER_BUFFER_SIZE];

// Define the host and the supported services
ERxHost host(sCmdBuffer, COMMAND_PARAMETER_BUFFER_SIZE, sResultBuffer, RESULT_BUFFER_SIZE);
ERxSystemService sysService(&host);
ERxUARTCmdReceiverService uartService(&Serial);
ERxMessageRouterService routerService(&host, &Serial, sRouterBuffer, ROUTER_BUFFER_SIZE);
ERxTHDetectionService thDetectionService(DHT11PIN);
ERxIRControllerService irService(RECV_PIN);

void setup() {

	Serial.begin(9600);

	// Set the address.
	host.SetMyAddress(MY_ADDRESS);
	routerService.SetRelayMessage(ROUTER_RELAY_ENABLE); // IMPORTANT: Only set true on the coordinator

	// Add service to host.
	host.AddService(&sysService);
	host.AddService(&uartService);
	host.AddService(&routerService);
	host.AddService(&thDetectionService);
	host.AddService(&irService);

	Serial.println("System startup");
}

void loop() {

	host.Run();
}