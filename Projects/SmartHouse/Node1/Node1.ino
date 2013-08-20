/*
Features
 * Smart house: Node 1. Address: 0x01
 * This node runs as a central controller of the XBee network. 
 * It provides APs (access point) for the external devices to communicate with the XBee network.
    * Web server, blue tooth, XBee.
    * It receives commands from the external devices, dispatches to the correponding node, 
    and returns the result to the external device where the command is from.
 * It publishes the data to the internet automatically. 
    * The IoT (Internet of Thing) server is cosm.com (a.k.a pachube which is required by the previous one)

Steps to deploy this sketch
 1. Configure the sketch as the Configuration section.
 2. Build and upload the sketch to aruino board.
 3. Wire the harwares as the Wire section.
 4. Power up the arduino board.

Test local commands
  * Help: "0x50 0x01 0xFA 0x01\r\n"
Test remote commands
  * Detect TH: "0x50 0x02 0xFA 0x40\r\n"
  * Send IR "0x50 0x02 0xFA 0x20 0xFF827D\r\n"

Configuration
 * Set MY_ADDRESS. Make sure it is unique in the smart house network. Use a different address if necessary.
 * Set ROUTER_RELAY_ENABLE. If the Ardunio connects to XBee coordinator, set it true. Otherwise, set it false.
 * Create a feed on the pacube.com and create 5 data streams with the ids 0, 1, 2, 3, 4.
 * Make sure the MAX_DATASTREAM_NUM defined in ERxPachube.h to be 5 or larger.
 * Fill in the correct PACHUBE_API_KEY and PACHUBE_FEED_ID in the macros below.
 * Set mac and ip. Make sure they don't conflict within the LAN. Use a different address if necessary.

Hardwares:
    Mega2560
    Ethernet Shield
    xBee (S1, coordinator)
    Bluetooth (slave) 
Services:
    System
    UARTCmdReceiver (one for xbee, the other for bluetooth)
    MessageRouter
    Pachube
    WebCmdReceiver

Circuit:
* Stack Ethernet shield over the Arduino Board
* Ethernet shield attached to pins 10, 11, 12, 13
* Connect the Ethernet shield to the router
* Stack the XBee coordinator

Requried library
	ERxPachube: http://code.google.com/p/pachubelibrary/
	Time
	ERxXxxx : http://code.google.com/p/easyrobot

* Created 3 June 2012
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
#include <ERxTextMessage.h>
#include <ERxHost.h>
#include <ERxService.h>
#include <ERxSystemService.h>
#include <ERxUARTCmdReceiverService.h>
#include <ERxMessageRouterService.h>
#include <ERxPachubeService.h>

//--------------CONFIGURATION-------------------------------------------------------------
// Macro definitions which require modifying for different harwares.
#define MY_ADDRESS 0x01
// ***IMPORTANT*** :Set true on the coordinator, and false on end device.
#define ROUTER_RELAY_ENABLE true

byte mac[] = { 0xCC, 0xAC, 0xBE, 0xEF, 0xFE, 0x99 }; // make sure this is unique on your network
byte ip[] = { 192, 168, 1, 178   };                  // no DHCP so we set our own IP address

#define PACHUBE_API_KEY				"YourKey" // Fill in your API key PACHUBE_API_KEY
#define PACHUBE_FEED_ID				42131 // Fill in your id

#define TH_SENSOR_ADDRESS			0x02 // The host address where the TH sensor is hosted

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
ERxPachubeService pachubeService(PACHUBE_API_KEY, PACHUBE_FEED_ID);

void setup() {

	Serial.begin(9600);
	Ethernet.begin(mac, ip); // Arduino hangs here when the ethernet shield isn't connected.

	// Set the address.
	host.SetMyAddress(MY_ADDRESS);
	routerService.SetRelayMessage(ROUTER_RELAY_ENABLE); // IMPORTANT: Only set true on the coordinator

	// Add service to host.
	host.AddService(&sysService);
	host.AddService(&uartService);
	host.AddService(&routerService);
	host.AddService(&pachubeService);

	pachubeService.SetSensorAddress(TH_SENSOR_ADDRESS);

	Serial.println("System startup");
}

void loop() {

	host.Run();
}