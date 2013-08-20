/**************************************************************************************
Functionality:	
	Test all the functions used for the communication protocol

The circuit:
	<Describe the used circuit connection.>

More:
	<Add more information regarding the hardware>

Occupied MCU resource:
	<Indicate which resources are occupied by this functionality. 
	When try to implement other functionalities, these resouces shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include <Arduino.h>
#include <HardwareSerial.h>
#include "SerialComHelper.h"
#include "ERxProtocol.h"

SerialComHelper comHepler;

void setup() 
{
	Serial.begin(9600);
}
 
 void loop() {

	 ERxInstructionFrame* pInst = comHepler.GetFirstInstruction(&Serial);
	 if(pInst != NULL && pInst->Valid())							// 3. Execute the command if a valid instruction command returned.
	 {
		 switch(pInst->GetCommand())
			{
		 case 0x01:
			 {
				 ERUINT8 data = comHepler.ReadParameterAsUChar(pInst);
				 const char* pStr = "uchar";
				 comHepler.SendInstruction(&Serial, 0x22,(ERINT8*)pStr);
				 comHepler.SendInstruction(&Serial, 0x11,data);
			 }

			 break;
		 case 0x02:
			 {
				 ERINT8 data = comHepler.ReadParameterAsChar(pInst);
				 const char* pStr = "char";
				 comHepler.SendInstruction(&Serial, 0x22,(ERINT8*)pStr);
				 comHepler.SendInstruction(&Serial, 0x12,data);
			 }

			 break;
		 case 0x03:
			 {
				 ERINT32 data = comHepler.ReadParameterAsLong(pInst);
				 if(255 == data) //0x FF-00-00-00 on intel CPU win OS
				 {
					 const char* pStr = "long";
					 comHepler.SendInstruction(&Serial, 0x22,(ERINT8*)pStr);
					 comHepler.SendInstruction(&Serial, 0x13,data);
				 }
			 }

			 break;
		 case 0x04:
			 {
				 ERFLOAT32 data = comHepler.ReadParameterAsFloat(pInst);
				 const char* pStr = "float";
				 comHepler.SendInstruction(&Serial, 0x22,(ERINT8*)pStr);
				 comHepler.SendInstruction(&Serial, 0x14,data);
			 }

			 break;

			 //case ...

		 default:
			 const char* pStr = "unknown";
			 comHepler.SendInstruction(&Serial, 0xFF,(ERINT8*)pStr);

			 comHepler.SendInstruction(&Serial, 0xAA, pInst->GetParameterStream()->GetBufferSize());
			 comHepler.SendInstruction(&Serial, pInst->GetCommand(), *(pInst->GetParameterStream()));
			 break;
			}
	 }

	 //ERUINT8 cmd = 0x03;
	 //ERINT32 para = 255;
	 //comHepler.SendInstruction(&Serial, cmd,para);

	 //delay(1000);
 }


