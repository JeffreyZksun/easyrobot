/**************************************************************************************
Functionality:	
	Receive the instruction from serial 0 to control the on and off status of LED.
	Return the execution result though serial 0.
	All the transported data uses the communication protocol to express the rich information.

The circuit:
	LED attached from pin 13 to ground.
	The USB cable is connected to PC to transport the data stream.

More:
	Serial: 0 (RX) and 1 (TX); 
	Serial 1: 19 (RX) and 18 (TX); 
	Serial 2: 17 (RX) and 16 (TX); 
	Serial 3: 15 (RX) and 14 (TX).  
	Used to receive (RX) and transmit (TX) TTL serial data. 
	Pins 0 and 1 are also connected to the corresponding pins of the FTDI USB-to-TTL Serial chip.

Occupied MCU resource:
	Serial port pins.

Author:	Jeffrey Sun
Created: 7/2010

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxCore.h>
#include "SerialComHelper.h"
#include "ERxProtocol.h"
SerialComHelper comHepler;

const int ledPin =  13;      // the number of the LED pin

// Example - Generate instruction frame and send through serial port
void ResponseResult()
{
	comHepler.SendInstruction(&Serial, 0x01,"OK");
}

// Example - Get instruction from serial port.
void CheckAndExecuteInstruction()
{
	bool bHasInstruction = true;

	while(bHasInstruction)
	{
		ERxInstructionFrame* pInst = comHepler.GetFirstInstruction(&Serial);
		if(pInst != NULL && pInst->Valid())							// 3. Execute the command if a valid instruction command returned.
		{
			switch(pInst->GetCommand())
			{
			case 0x11:
				{
					ERxParameterStream* pPara = pInst->GetParameterStream();
					ERINT8 data = 0;
					pPara->Read(&data);

					//ERINT8 data = comHepler.ReadParameterAsChar(pInst);

					if(data != 0)
						digitalWrite(ledPin, HIGH); 
					else
						digitalWrite(ledPin, LOW); 	
				}					
			
				break;

				//case ...

			default:
				break;
			}

			ResponseResult();
		}
		else
		{
			bHasInstruction = false;
		}
	}	
}

void setup() {

	// initialize the LED pin as an output:
	pinMode(ledPin, OUTPUT);    
	digitalWrite(ledPin, HIGH);

	Serial.begin(9600); 
}

void loop() {

	CheckAndExecuteInstruction();

	delay(1000);
}

