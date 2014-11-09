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
PC --> UART --> ERxTextMessage --> ERxUARTCmdReceiverService --> ERxMessageRouterService (override result stream) --> ERxHost.Execute() --> ERxMotorService -->
PC <-- UART <-- ERxRedirectOutputStream <------------------------------------------------- (execution result)

The circuit:
---------------------------------------------
	Arduino		L9110(Motor Driver)			
	2				IA1
	3				IB1
	4				IA2
	5				IB2

	GND				GND

				LED
	6			 led1
	7 			 led2
	8 			 led3
	GND			 GND

				Bluetooth
	5V			 	VCC
	GND		 	 	GND
	0(TX)    		RX
	1(RX)			TX
	9 				STATE
	
---------------------------------------------
	Battery	 	  L9110	 	Ardiono
	GND				GND		  GND
	+9V				VCC		  Vin

---------------------------------------------
	L9110		Motor
	MotorA		DC Motor
	MotorB		DC Motor
---------------------------------------------


* Created 29 November 2014
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
#define IA1 2  // Left motor
#define IB1 3
#define IA2 4  // Right motor         
#define IB2 5
#define LED1 6
#define LED2 7
#define LED3 8
#define LED_COUNT 3 // the total number of leds.
#define BTSTATE 9

ERxL9110 leftMotor(IA1, IB1);
ERxL9110 rightMotor(IA2, IB2);

bool isBluetoothConnected = false;
byte ledPin[]={LED1, LED2, LED3};
unsigned long ledChangeTime;

// forard declare
void updateBlueToothStatus();
void updateLED();

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

	pinMode(BTSTATE, INPUT);

	// led
	for(int x=0;x<LED_COUNT;x++){
    	pinMode(ledPin[x],OUTPUT);
	}
    ledChangeTime=millis();
}

void loop() {

	host.Run();
	updateBlueToothStatus();
	updateLED();
}

void updateBlueToothStatus(){
	if(digitalRead(BTSTATE) == HIGH){
		isBluetoothConnected = true;
	}
	else{
		isBluetoothConnected = false;
	}
}

bool sideLedOn=false;
// Always light on the middle, flash the other two.
void changeLEDMode1(){
    digitalWrite(LED2,HIGH);// light on the middle one.
    digitalWrite(LED1,sideLedOn?HIGH:LOW);
    digitalWrite(LED3,sideLedOn?HIGH:LOW);
    sideLedOn = !sideLedOn;
}

int currentLED=0;
void changeLEDMode2(){
    for(int x=0;x<LED_COUNT;x++){
    	digitalWrite(ledPin[x],LOW);//熄灭
    }
    digitalWrite(ledPin[currentLED],HIGH);//设当前LED高平信号，点亮
    currentLED+=1; //Next one
    if(currentLED==LED_COUNT) 
    {
    	currentLED=0;
    }
}

int ledDelay=300;
void updateLED(){
	if ((millis()-ledChangeTime)>ledDelay){
		if(isBluetoothConnected){
			changeLEDMode1();
		}
		else{
     		changeLEDMode2();
		}
        ledChangeTime=millis();
    }
}