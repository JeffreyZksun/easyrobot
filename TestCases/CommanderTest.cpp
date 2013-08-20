/**************************************************************************************
Functionality:	
	Receive the command from IR and UART and send it back to UART.

The circuit:
	Connect the IR receiver with pin#11.
	Connect the USB to PC. We use Serial 0 at pin#0 & 1.

More:
<Add more information regarding the hardware>

Occupied MCU resource:
<Indicate which resources are occupied by this functionality. 
When try to implement other functionalities, these resources shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include <Arduino.h>
#include "ERxCommander.h"
#include "ERxProtocol.h"
#include "SerialComHelper.h"
#include "ERxUARTCommandRecv.h"
#include <IRremote.h>
#include "ERxIRCommandRecv.h"
#include "ERxNew.h"

int RECV_PIN = 11; 

ERxCommander cmder;
ERxUARTCommandRecv uartRecv(&Serial);
ERxIRCommandRecv irRecv(RECV_PIN);
SerialComHelper comHepler;

void setup() {

	// initialization
	Serial.begin(9600);
	irRecv.Begin();

	cmder.RegisterCommandReceiver(&uartRecv);
	cmder.RegisterCommandReceiver(&irRecv);
}

void loop() {

	// Do the things.
	ERxCommand cmd = cmder.GetNextPendingCommand();
	ERUINT8 cmdId = cmd.CommandID();
	switch(cmdId)
	{
	case 0x10:
		comHepler.SendInstruction(&Serial, cmdId, "Stop");
		break;
	case 0x11:
		comHepler.SendInstruction(&Serial, cmdId, "RunForward");
		break;
	case 0x12:
		comHepler.SendInstruction(&Serial, cmdId, "TurnLeft");
		break;
	case 0x13:
		comHepler.SendInstruction(&Serial, cmdId, "TurnRight");
		break;
	case 0x14:
		comHepler.SendInstruction(&Serial, cmdId, "RunBackward");
		break;
	default:
		comHepler.SendInstruction(&Serial, cmdId, "default");
		break;
	}

	delay(1000);
}
