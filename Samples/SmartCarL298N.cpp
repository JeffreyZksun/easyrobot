#include <ERxCore.h>

/**************************************************************************************
Functionality:	
	Control the small car automatically or manually.
	There are two control models, automatic and manual.

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
#include "Servo.h"
#include "SerialComHelper.h"
#include "ERxProtocol.h"
SerialComHelper comHepler;

const int ledPin =  13; // output pin
boolean bIsLedOpen = false;

//const char LeftMotorPinA = 4; // output pin control left motor
//const char LeftMotorPinB = 5;
//
//const char RightMotorPinA = 6; // output pin control right motor
//const char RightMotorPinB = 7;

const char FrontBumperSensorPin = 3; // input pin

volatile int initialSensorStatus = 0;

boolean bIsAutomaticModel = false; // indicate the control model


// Motor control
int E1 = 4;  // Left motor
int M1 = 5;
int E2 = 6;  // Right motor         
int M2 = 7;   

int speed = 255;

// Forward declares
void RunForward();
void TurnLeft();
void TurnRight();
void RunBackward();
void Stop();
void SetMotorSpeed();
void UpdateLedStatus();
void RunAutomatically();
void ExecuteInstrunction(ERxInstructionFrame* pInstr);
boolean IsHitTheWall();
int DigitalReadWithCheck(char pin);


void setup() {

	// initialization
	Serial.begin(9600);

	pinMode(ledPin, OUTPUT);

	//pinMode(LeftMotorPinA, OUTPUT);
	//pinMode(LeftMotorPinB, OUTPUT);

	//pinMode(RightMotorPinA, OUTPUT);
	//pinMode(RightMotorPinB, OUTPUT);

	pinMode(FrontBumperSensorPin, INPUT);

	pinMode(M1, OUTPUT);  
	pinMode(M2, OUTPUT);


	delay(500); // Wait the processor become steady.
	initialSensorStatus = DigitalReadWithCheck(FrontBumperSensorPin);

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
					initialSensorStatus = DigitalReadWithCheck(FrontBumperSensorPin); // Fresh the data.
					Stop();
					comHepler.SendInstruction(&Serial, 0xFF, "Change control model to Manual");
				}
				return;
			}
		case 0x02: // Query control model
			if(bIsAutomaticModel)
				comHepler.SendInstruction(&Serial, 0xFF, "Automatic Model");
			else
				comHepler.SendInstruction(&Serial, 0xFF, "Manual Model");
			return;
		}
	 }
	 

	 if(bIsAutomaticModel)
		 RunAutomatically();
	 else if(pInstr != NULL)
		 ExecuteInstrunction(pInstr);

	UpdateLedStatus();

	//delay(1000);
}

void RunAutomatically()
{
	if(IsHitTheWall())
	{
		RunBackward();
		delay(2000);
		TurnLeft();
		delay(2000);
	}

	RunForward();
}

void ExecuteInstrunction(ERxInstructionFrame* pInstr)
{
	if(NULL == pInstr)
		return;

	switch (pInstr->GetCommand())
	{
	case 0x11:
		RunForward();
		break;
	case 0x12:
		TurnLeft();
		break;
	case 0x13:
		TurnRight();
		break;
	case 0x14:
		RunBackward();
		break;
	default:
		Stop();
		break;
	}
}

int DigitalReadWithCheck(char pin)
{
	// Use read after read to avoid the signal shake.
	int waitTime = 2;
	int lastStatus2 = digitalRead(pin);
	delay(waitTime);
	int lastStatus1 = digitalRead(pin);

	int i = 10;
	while(i--)
	{
		delay(waitTime);
		int curStatus = digitalRead(pin);
		if(curStatus == lastStatus1 && curStatus == lastStatus2)
			return curStatus; // Return when the two continuous read are the same.
		else
		{
			lastStatus2 = lastStatus1;
			lastStatus1 = curStatus;
		}
	}

	comHepler.SendInstruction(&Serial, 0x11, "Error when read digital pin.");
}

boolean IsHitTheWall()
{
	int curStatus = DigitalReadWithCheck(FrontBumperSensorPin);

	return (curStatus != initialSensorStatus);
}
//
//void setMotorPins(boolean isE1High, boolean isM1High, boolean isE2High, boolean isM2High)
//{
//	analogWrite(E1, 200);
//	analogWrite(E2, 200);
//
//	digitalWrite(M1, isM1High);
//	digitalWrite(E2, isE2High);	
//}
//
//void setMotorPins(boolean isLAHigh, boolean isRAHigh)
//{
//	setMotorPins(isLAHigh, !isLAHigh, isRAHigh, !isRAHigh);
//}

void RunForward()
{
	digitalWrite(M1, 1);
	digitalWrite(M2, 1);

	SetMotorSpeed();
	
	comHepler.SendInstruction(&Serial, 0xFF, "RunForward");
}


void TurnLeft()
{
	digitalWrite(M1, 0);
	digitalWrite(M2, 1);

	SetMotorSpeed();
	
	comHepler.SendInstruction(&Serial, 0xFF, "TurnLeft");
}

void TurnRight()
{
	digitalWrite(M1, 1);
	digitalWrite(M2, 0);

	SetMotorSpeed();
	
	comHepler.SendInstruction(&Serial, 0xFF, "TurnRight");
}

void RunBackward()
{
	digitalWrite(M1, 0);
	digitalWrite(M2, 0);

	SetMotorSpeed();

	comHepler.SendInstruction(&Serial, 0xFF, "RunBackward");
}

void Stop()
{
	//setMotorPins(LOW, LOW, LOW, LOW);

	//pinMode(E1, INPUT);  
	//pinMode(E2, INPUT);

	//analogWrite(E1, 0);
	//analogWrite(E2, 0);
	digitalWrite(E1, LOW);
	digitalWrite(E2, LOW);

	comHepler.SendInstruction(&Serial, 0xFF, "Stop");
}

void SetMotorSpeed()
{
	analogWrite(E1, speed);
	analogWrite(E2, speed);
}

void UpdateLedStatus()
{
	bIsLedOpen = !bIsLedOpen;
	digitalWrite(ledPin, bIsLedOpen);
}

