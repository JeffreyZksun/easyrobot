/*

Update data to pacuhbe server each minute.

Steps to run this sketch
 1. Connect the Circuit (If there is no ethernet shield, the program will hang)
 2. Input the correct kye and feed id. (Create a fead with 5 data streams)
 3. Build the sketch and upload to Arduino
 4. check the result in pachube.

Required hardware
	Arduino
	Ethernetshield

Circuit:
* Stack Ethernet shield over the Arduino Board
* Ethernet shield attached to pins 10, 11, 12, 13
* Connect the Ethernet shield to the router

* Created 1 June 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Time.h>
#include <ERxPachube.h>
#include <ERxIOStream.h>
#include <ERxOutputStream.h>
#include <ERxHost.h>
#include <ERxService.h>
#include <ERxCommandId.h>
#include <ERxCommandExecutorService.h>
#include <ERxPachubeService.h>

byte mac[] = { 0xCC, 0xAC, 0xBE, 0xEF, 0xFE, 0x99 }; // make sure this is unique on your network
byte ip[] = { 192, 168, 1, 178   };                  // no DHCP so we set our own IP address

#define PACHUBE_API_KEY				"YouKey" // Fill in your API key PACHUBE_API_KEY
#define PACHUBE_FEED_ID				23408 // Fill in your id

static char* pachubeData = "12.55 13.25 18.65 55.34\r\n";

// Allocate the buffers
#define COMMAND_PARAMETER_BUFFER_SIZE 128
#define RESULT_BUFFER_SIZE 128
static uint8_t sCmdBuffer[COMMAND_PARAMETER_BUFFER_SIZE];
static uint8_t sResultBuffer[RESULT_BUFFER_SIZE];


// Define the host and the supported services
ERxHost host(sCmdBuffer, COMMAND_PARAMETER_BUFFER_SIZE, sResultBuffer, RESULT_BUFFER_SIZE);
ERxCommandExecutorService cmdExeService(TH_GET, pachubeData);
ERxPachubeService pachubeService(PACHUBE_API_KEY, PACHUBE_FEED_ID);

void setup() {

	Serial.begin(9600);
	Ethernet.begin(mac, ip); // Arduino hangs here when the ethernet shield isn't connected.

	// Set the address.
	host.SetMyAddress(0x01);

	// Add service to host.
	host.AddService(&cmdExeService);
	host.AddService(&pachubeService);

	Serial.println("System startup");

}

void loop() {

	host.Run();

}