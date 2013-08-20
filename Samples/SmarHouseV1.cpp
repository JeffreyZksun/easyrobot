#include <ERxCore.h>

/**************************************************************************************
Functionality:	
<Describe the main functionality of the code in this file.>

The circuit:
	Connect the LED at pin#8. Send command 0x21.0x010x01;0x21.0x010x00 to control the LED.

More:
<Add more information regarding the hardware>

Occupied MCU resource:
<Indicate which resources are occupied by this functionality. 
When try to implement other functionalities, these resources shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
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

/************************************************************************/
//  CMD     PARAMETERS                                          COMMENTS                                                              
//  0x21    ERUINT8 ERUINT8        CHANGE_SWITCH_CMD, Digital I/O index, status(1: open, 0: close)
/************************************************************************/
#define CHANGE_SWITCH_CMD 0x21 


void setup() {

	// initialization
	Serial.begin(9600);
	irRecv.Begin();

	cmder.RegisterCommandReceiver(&uartRecv);
	cmder.RegisterCommandReceiver(&irRecv);

	//digitalWrite(8, 1);
}

void loop() {

	// Do the things.
	ERxCommand cmd = cmder.GetNextPendingCommand();
	ERUINT8 cmdId = cmd.CommandID();
	if(0 == cmdId) // Invalid command.
	{
		delay(10);
		return;
	}

	ERxParameterStream* pParameter = cmd.Parameter();
	switch(cmdId)
	{
	case CHANGE_SWITCH_CMD: // digital I/O control
		{
			comHepler.SendInstruction(&Serial, cmdId, "digital I/O control");

			while(!pParameter->Empty())
			{
				ERUINT8 digitalPinIndex = 0;
				ERUINT8 pinStatus = 0;
				pParameter->Read(&digitalPinIndex);
				pParameter->Read(&pinStatus);

				//ERUINT8 c = digitalPinIndex + 48;
				//comHepler.SendInstruction(&Serial, cmdId, c);

				digitalWrite(digitalPinIndex, pinStatus);
			}
			break;
		}
	
	default:
		comHepler.SendInstruction(&Serial, cmdId, "default");
		break;
	}
}
