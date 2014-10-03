/*
Simple host

Start a host with basic services.

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Open serial monitor 
 3. Send string "0x50 0x01 0x02 0x01\r\n" or "80 1 2 1\r\n" to Arduino to execute the command 0x01
 Verify the output string is "80 2 1 2 1 xxxxx\r\n" or the equivalent string. xxxx stands for the help message.
 4. Send string "0x50 0x08 0x02 0x01\r\n" or "80 8 2 1\r\n".to router the command frame.
 Verify the output string is "0x50 0x08 0x02 0x01\r\n" or the equivalent string, since the command is routed back to PC.

Data flow
PC --> UART --> ERxTextMessage --> ERxUARTCmdReceiverService --> ERxMessageRouterService (override result stream) --> ERxHost.Execute() --> Specific service -->
PC <-- UART <-- ERxRedirectOutputStream <------------------------------------------------- (execution result)

* Created 29 May 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxIOStream.h>
#include <ERxOutputStream.h>
#include <ERxTextMessage.h>
#include <ERxHost.h>
#include <ERxService.h>
#include <ERxSystemService.h>
#include <ERxUARTCmdReceiverService.h>
#include <ERxMessageRouterService.h>

//--------------CONFIGURATION-------------------------------------------------------------
// Macro definitions which require modifying for different harwares.
#define MY_ADDRESS 0x01
// ***IMPORTANT*** :Set true on the coordinator, and false on end device.
#define ROUTER_RELAY_ENABLE true

//-----------------------------------------------------------------------------------------

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


void setup() {

	Serial.begin(9600);

	// Set the address.
	host.SetMyAddress(MY_ADDRESS);
	routerService.SetRelayMessage(ROUTER_RELAY_ENABLE); // IMPORTANT: Only set true on the coordinator

	// Add service to host.
	host.AddService(&sysService);
	host.AddService(&uartService);
	host.AddService(&routerService);

	Serial.println("System startup");
}

void loop() {

	host.Run();
}