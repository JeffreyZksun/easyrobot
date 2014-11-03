/*
This a program for a two wheels driven robot car. 
The car has four wheels. There is one driving wheel on each side. 
The Motor is DC motor which is controlled by L9110.

The UART can receive the remote command to control the car.

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Open serial monitor 
 3. Send control commands from controller (PC) to controll the car via UART.

Control Commands (They are remote commands. So add the router prefix "0x50 0x01 0x02".)
Forward: "0x50 0x01 0x02 0x70 1\r\n"
Backward: "0x50 0x01 0x02 0x70 2\r\n"
Turn right: "0x50 0x01 0x02 0x70 3\r\n"
Turn Left: "0x50 0x01 0x02 0x70 4\r\n"
Stop: "0x50 0x01 0x02 0x70 5\r\n"
Help: "0x50 0x01 0x02 0x01\r\n"

Command flow
PC --> UART --> ERxTextMessage --> ERxUARTCmdReceiverService --> ERxMessageRouterService (override result stream) --> ERxHost.Execute() --> ERxL9110MotorService -->
PC <-- UART <-- ERxRedirectOutputStream <------------------------------------------------- (execution result)

The circuit:
---------------------------------------------
	Arduino		L9110(Motor Driver)			
	4				IA1
	5				IB1
	6				IA2
	7				IB2

	GND				GND
	
---------------------------------------------
	Motor Battery	   L9110	 Ardiono
	GND					GND		  GND
	+2.5~12V			VCC		  N/A

---------------------------------------------
	Logic Battery	   	 Ardiono
	GND					  GND
	+7~12V			  	  Vin

The motor can use the same power with the circuit.

---------------------------------------------
	L9110		Motor
	MotorA			DC Motor
	MotorB			DC Motor
---------------------------------------------

If you want to communicate with Arduino via Bluetooth other than USB cable,
connect the bluetooth module with the wiring below. (Don't need to change the code)

Arduino 	Bluetooth
 5V			 VCC
 GND		 GND
 TX    		 RX
 RX			 TX

* Created 2 November 2014
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
#include <ERxIMotor.h>
#include <ERxL9110.h>
#include <ERxMotorService.h>

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
ERxMotorService motorService;

// Wiring
// The wiring pins to the motor controller. All pins are output.
#define IA1 4  // Left motor
#define IB1 5
#define IA2 6  // Right motor         
#define IB2 7

ERxL9110 leftMotor(IA1, IB1);
ERxL9110 rightMotor(IA2, IB2);

void setup() {

	Serial.begin(9600);
	Serial.println("System startup");

	// Set the address.
	host.SetMyAddress(MY_ADDRESS);
	routerService.SetRelayMessage(ROUTER_RELAY_ENABLE); // IMPORTANT: Only set true on the coordinator

	// Add service to host.
	host.AddService(&sysService);
	host.AddService(&uartService);
	host.AddService(&routerService);
	host.AddService(&motorService);
	motorService.addLeftMotor(&leftMotor);
	motorService.addRightMotor(&rightMotor);
}

void loop() {

	host.Run();
}