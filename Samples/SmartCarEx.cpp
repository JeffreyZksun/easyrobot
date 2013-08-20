#include <ERxCore.h>

/**************************************************************************************
Functionality:	
	Control the small car automatically or manually.
	There are two control models, automatic and manual.

The circuit:
	Connect the sensor to digital pin 3.
	Connect left motor to digital pin 6.
	Connect right motor to digital pin 7.

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

#define FrontBumperSensorPin  3 // input pin
#define leftLedPin 4
#define rightLedPin 5
#define leftMotorPin 6
#define rightMotorPin 7


//#define  ForwardLeftSpeed 1700
//#define  ForwardRightSpeed 1300
//#define  BackwardLeftSpeed 1300
//#define  BackwardRightSpeed 1700

#define MechanicalDesignA // Servo connects to wheel directly

#ifdef MechanicalDesignA
	#define  ForwardLeftSpeedStep -40
	#define  ForwardRightSpeedStep 40
	#define  BackwardLeftSpeedStep 40
	#define  BackwardRightSpeedStep -40
#else
	#define  ForwardLeftSpeedStep 40
	#define  ForwardRightSpeedStep -40
	#define  BackwardLeftSpeedStep -40
	#define  BackwardRightSpeedStep 40
#endif

#define  SpeedChangeLoop 5

const int ledPin =  13; // output pin
boolean bIsLedOpen = false;
volatile int initialSensorStatus = 0;


boolean bIsAutomaticModel = false; // indicate the control model

// Motor control
#define MAX_DUTY 2000
#define MIN_DUTY 1000

Servo leftMotor;
Servo rightMotor;

// Forward declares
void RunForward();
void TurnLeft();
void TurnRight();
void RunBackward();
void Stop();
void SetMotorSpeed(int leftSpeed, int rightSpeed);
void SetMotorSpeedByStep(int leftSpeedStep, int rightSpeedStep, int loopNumber);
void UpdateLedStatus(bool bLeftOn, bool bRightOn);
void UpdateLedStatus();
void RunAutomatically();
void ExecuteInstrunction(ERUINT8 cmdID);
boolean IsHitTheWall();
int DigitalReadWithCheck(char pin);


void setup() {

	// initialization
	Serial.begin(9600);
	irRecv.Begin();

	cmder.RegisterCommandReceiver(&uartRecv);
	cmder.RegisterCommandReceiver(&irRecv);

	pinMode(ledPin, OUTPUT);
	pinMode(leftLedPin, OUTPUT);
	pinMode(rightLedPin, OUTPUT);

	leftMotor.attach(leftMotorPin, MIN_DUTY, MAX_DUTY);
	rightMotor.attach(rightMotorPin, MIN_DUTY, MAX_DUTY);

	Stop();

	delay(500); // Wait the processor become steady.
	initialSensorStatus = DigitalReadWithCheck(FrontBumperSensorPin);

}
 
void loop() {

	 //ERxInstructionFrame* pInstr = comHepler.GetFirstInstruction(&Serial);
	ERxCommand cmd = cmder.GetNextPendingCommand();
	ERUINT8 cmdId = cmd.CommandID();
	 //if(pInstr != NULL) 
	 //{
		switch (cmdId)
		{
		case 0x01:	// Setting the model
			{
				bIsAutomaticModel = true;
				comHepler.SendInstruction(&Serial, 0xFF, "Change control model to Automatic");
				return;
			}
		case 0x02:
			{	
				bIsAutomaticModel = false;

				initialSensorStatus = DigitalReadWithCheck(FrontBumperSensorPin); // Fresh the data.
				Stop();
				comHepler.SendInstruction(&Serial, 0xFF, "Change control model to Manual");

				return;
			}
		case 0x03: // Query control model
			if(bIsAutomaticModel)
				comHepler.SendInstruction(&Serial, 0xFF, "Automatic Model");
			else
				comHepler.SendInstruction(&Serial, 0xFF, "Manual Model");
			return;
		}
	 //}
	 

	 if(bIsAutomaticModel)
		 RunAutomatically();
	 else if(cmdId != 0)
		 ExecuteInstrunction(cmdId);

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

void ExecuteInstrunction(ERUINT8 cmdID)
{

	switch (cmdID)
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


void RunForward()
{
	//SetMotorSpeed(ForwardLeftSpeed, ForwardRightSpeed);//SetMotorSpeed(1800, 1200);
	UpdateLedStatus(true, true);
	SetMotorSpeedByStep(ForwardLeftSpeedStep, ForwardRightSpeedStep, SpeedChangeLoop);

	comHepler.SendInstruction(&Serial, 0xFF, "RunForward");
}

void TurnLeft()
{
	//SetMotorSpeed(1200, 1200);
	//SetMotorSpeed(BackwardLeftSpeed, ForwardRightSpeed);
	UpdateLedStatus(true, false);
	SetMotorSpeedByStep(BackwardLeftSpeedStep, ForwardRightSpeedStep, SpeedChangeLoop);

	comHepler.SendInstruction(&Serial, 0xFF, "TurnLeft");
}

void TurnRight()
{
	//SetMotorSpeed(1800, 1800);
	//SetMotorSpeed(ForwardLeftSpeed, BackwardRightSpeed);
	UpdateLedStatus(false, true);
	SetMotorSpeedByStep(ForwardLeftSpeedStep, BackwardRightSpeedStep, SpeedChangeLoop);
	
	comHepler.SendInstruction(&Serial, 0xFF, "TurnRight");
}

void RunBackward()
{
	//SetMotorSpeed(1200, 1800);
	//SetMotorSpeed(BackwardLeftSpeed, BackwardRightSpeed);
	UpdateLedStatus(true, true);
	SetMotorSpeedByStep(BackwardLeftSpeedStep, BackwardRightSpeedStep, SpeedChangeLoop);

	comHepler.SendInstruction(&Serial, 0xFF, "RunBackward");
}

void Stop()
{	
	SetMotorSpeed(1500,1500);
	//pinMode( leftMotorPin, INPUT) ;
	//pinMode( rightMotorPin, INPUT) ;
	UpdateLedStatus(false, false);

	comHepler.SendInstruction(&Serial, 0xFF, "Stop");
}

void SetMotorSpeed(int leftSpeed, int rightSpeed)
{
	//pinMode( leftMotorPin, OUTPUT) ;
	//pinMode( rightMotorPin, OUTPUT) ;
	leftMotor.write(leftSpeed);
	rightMotor.write(rightSpeed);
}

void SetMotorSpeedByStep(int leftSpeedStep, int rightSpeedStep, int loopNumber)
{
	int leftSpeed = 1500;
	int rightSpeed = 1500;
	for(int i = 0; i < loopNumber; i++)
	{
		leftSpeed += leftSpeedStep; 
		rightSpeed += rightSpeedStep; 
		SetMotorSpeed(leftSpeed, rightSpeed);
		delay(100);
	}
}

void UpdateLedStatus(bool bLeftOn, bool bRightOn)
{
	digitalWrite(leftLedPin, bLeftOn);
	digitalWrite(rightLedPin, bRightOn);
}

void UpdateLedStatus()
{
	bIsLedOpen = !bIsLedOpen;
	digitalWrite(ledPin, bIsLedOpen);
}
