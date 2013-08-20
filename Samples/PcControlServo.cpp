#include <ERxCore.h>

/**************************************************************************************
Functionality:	
	Receive the instruction from PC and set the servo position.

The circuit:
	Servo is connected to pin 9

More:
	Instruction from PC: 
		0x01: switch the control model.0 - automatic, others - manual.
		0x11: set the servo degree. [5,25] for 180 degrees. [10,20] for 90 degrees.

Occupied MCU resource:


Author:	Jeffrey Sun
Created: 7/2010

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include "Servo.h"
#include "SerialComHelper.h"
#include "ERxProtocol.h"
SerialComHelper comHepler;
Servo myservo;   // Define the servo

// The minimum and maximum value depends on the type of the servo.
int SERVO_MIN_DEGREE = 500; // -90 degrees
int SERVO_MAX_DEGREE = 2500; // +90 degrees

boolean bIsAutomaticModel = false; // indicate the control model
int pos = 1500;
int step = 100;
bool bIsForward = true;

void SetServoPosition()
{
	if(pos > SERVO_MAX_DEGREE)
	{
		pos = SERVO_MAX_DEGREE;
		bIsForward = false;
	}
	else if(pos < SERVO_MIN_DEGREE)
	{
		pos = SERVO_MIN_DEGREE;
		bIsForward = true;
	}

	myservo.writeMicroseconds(pos);
}

void RunAutomatically()
{
	if(bIsForward)
		pos += step;
	else
		pos -= step;

	SetServoPosition();
}

void ExecuteInstrunction(ERxInstructionFrame* pInstr)
{
	if(NULL == pInstr)
		return;

	switch (pInstr->GetCommand())
	{
	case 0x11:
		{
			char para = comHepler.ReadParameterAsUChar(pInstr); // the value should between SERVO_MIN_DEGREE / 100 and SERVO_MAX_DEGREE / 100
			pos = para * 100;
			SetServoPosition();
			comHepler.SendInstruction(&Serial, 0x01,"OK");
			break;
		}
	default:		
		break;
	}
}

void loop() {

	ERxInstructionFrame* pInstr = comHepler.GetFirstInstruction(&Serial);
	if(pInstr != NULL) 
	{
		switch (pInstr->GetCommand())
		{
		case 0x01:	// Setting the model
			{
				char model = comHepler.ReadParameterAsChar(pInstr); // 0 - automatic, others - manual.
				bIsAutomaticModel = (model == 0);

				if(bIsAutomaticModel)
					comHepler.SendInstruction(&Serial, 0xFF, "Change control model to Automatic");
				else
				{
					comHepler.SendInstruction(&Serial, 0xFF, "Change control model to Manual");
				}
				return;
			}
		}
	}


	if(bIsAutomaticModel)
		RunAutomatically();
	else if(pInstr != NULL)
		ExecuteInstrunction(pInstr);

	delay(100);
}

void setup() {

	Serial.begin(9600); 

	myservo.attach(9, SERVO_MIN_DEGREE, SERVO_MAX_DEGREE);				// Connect the servo the pin #9.
	myservo.writeMicroseconds(pos); 
}
