#include <ERxCore.h>

/**************************************************************************************
Functionality:	
	
	Security
	The robot will stop automatically when no any valid command is received.

	Low Power Consumption
	When there is no any valid command is received for a specified time, the robot will sleep to reduce
	the power consumption. It can be waken up by the UART communication.

The circuit:
---------------------------------------------
	Mega		L298N(Motor Driver)			
	4				E1
	5				M1
	6				E2
	7				M2

	GND				GND
	VIN(9V)			VD
---------------------------------------------
	Battery (for motor)		L298N	 Mega			Voltage Divider
	GND						GND		  GND				GND
	+9V						VS		  Jack(or Vin)		+
---------------------------------------------
	L298N		Motor
	M1			Lefter motor
	M2			Right motor
---------------------------------------------
	Mega		IR
	11			D
	GND			GND
	VCC(5V)		VCC
---------------------------------------------
	Mega		Voltage Divider
	A0			Signal
	GND			GND
---------------------------------------------
More:
	<Add more information regarding the hardware>

Occupied MCU resource:
	<Indicate which resources are occupied by this functionality. 
	When try to implement other functionalities, these resouces shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include <avr/power.h>
#include <avr/sleep.h>
#include "ERxCommander.h"
#include "ERxProtocol.h"
#include "SerialComHelper.h"
#include "ERxUARTCommandRecv.h"
#include <IRremote.h>
#include "ERxIRCommandRecv.h"
#include "ERxNew.h"

/************************************************************************/
//  CMD     PARAMETERS					COMMENTS                                                              
//  0x11		N/A					   RUN_FORWARD_CMD
//  0x12		N/A					   TURN_LEFT_CMD
//  0x13		N/A					   TRUN_RIGHT_CMD
//  0x14		N/A					   RUN_BACKFARD_CMD
//  0x15		N/A					   STOP_CMD
//  0x20		N/A					   QUERY_STATUS
//  0x21		ERFLOAT32			   REPLY_STATUS
/************************************************************************/

#define RUN_FORWARD_CMD		0x11
#define TURN_LEFT_CMD		0x12
#define TRUN_RIGHT_CMD		0x13
#define RUN_BACKFARD_CMD	0x14
#define STOP_CMD			0x15

#define QUERY_STATUS		0x20
#define REPLY_STATUS		0x21


#define ENABLESTATUSLOG

const ERINT8 ledPin =  13; // output pin
boolean bIsLedOpen = false;

volatile ERINT8 initialSensorStatus = 0;

boolean bIsAutomaticModel = false; // indicate the control model

ERINT8 speed = 255;
ERUINT8 currentStatus = STOP_CMD; // Use the command ID to identify the robot status.

// Wiring
// The wiring pins to the motor controller. All pins are output.
ERINT8 E1 = 4;  // Left motor
ERINT8 M1 = 5;
ERINT8 E2 = 6;  // Right motor         
ERINT8 M2 = 7; 
// IR receiver
ERINT8 RECV_PIN = 11; // input pin
// Sensors
const ERUINT8 FrontBumperSensorPin = 3; // input pin

// Security
// Automatically stop the robot, in case the communication between the robot and the remote controller is disconnected.
bool bEnableTimeout = true;
ERUINT32 stopTimeout = 5000; // milliseconds, after time out, the robot will stop automatically.
ERUINT32 lastCommandTime = 0; // milliseconds, record the time when the last valid command is received.

// Power
ERUINT32 sleepTimeout = 10000; //  milliseconds, sleep the robot.

ERxCommander cmder;
ERxUARTCommandRecv uartRecv(&Serial);
ERxIRCommandRecv irRecv(RECV_PIN);
SerialComHelper comHepler;

// Forward declares
void RunForward();
void TurnLeft();
void TurnRight();
void RunBackward();
void Stop();
void SetMotorSpeed();
void UpdateLedStatus();
void RunAutomatically();
void ExecuteInstrunction(ERxCommand& cmd);
ERUINT8 IsHitTheWall();
ERINT8 DigitalReadWithCheck(ERUINT8 pin);
void sleepNow();


void setup() {

	// initialization
	// initialization
	Serial.begin(9600);
	irRecv.Begin();

	cmder.RegisterCommandReceiver(&uartRecv);
	cmder.RegisterCommandReceiver(&irRecv);

	pinMode(ledPin, OUTPUT);

	pinMode(FrontBumperSensorPin, INPUT);

	pinMode(M1, OUTPUT);  
	pinMode(M2, OUTPUT);


	delay(500); // Wait the processor become steady.
	initialSensorStatus = DigitalReadWithCheck(FrontBumperSensorPin);

}
 
void loop() {

	ERxCommand cmd = cmder.GetNextPendingCommand();
	ERUINT8 cmdId = cmd.CommandID();
	if(0 == cmdId) // Invalid command.
	{
		if(bEnableTimeout)
		{
			ERUINT32 currentTime = millis();
			ERINT32 pastTime = currentTime - lastCommandTime;
			if(pastTime > 0)
			{
				if(pastTime > stopTimeout)
				{
					// Don't need to execute the stop operation every time.
					if(STOP_CMD != currentStatus || pastTime < 4*stopTimeout)
						Stop();
				}

				// There is some issues when the xbee is connected.
				// 1. Can't get data from serial 1 through USB correctly.
				// 2. Can't week up after sleeping.
				// 
				//if(pastTime > sleepTimeout)
				//{
				//	sleepNow();
				//	lastCommandTime = millis();
				//}
			}
			else // The timer overflow.
			{
				lastCommandTime = millis();
			}


			//Serial.print("current: ");
			//Serial.println(currentTime);
			//Serial.print("last: ");
			//Serial.println(lastCommandTime);
		}
		delay(1000);
		return;
	}

	// Update the last command status.
	if(bEnableTimeout)
	{
		lastCommandTime = millis();
		//Serial.print("last: ");
		//Serial.println(lastCommandTime);
	}
		
	switch (cmdId)
	{
	case 0x01:	// Setting the model
		{
			ERxParameterStream* pParameter = cmd.Parameter();
			ERUINT8 model = 0;
			pParameter->Read(&model); // 0 - automatic, others - manual.
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

	case QUERY_STATUS: // query battery status
		{
			ERxParameterStream paraStream;
			ERFLOAT32 a0 = analogRead(0);
			ERFLOAT32 batteryVoltage = a0 / 102.4; // A0 / 1024 * 5V * 2
			paraStream.Write(batteryVoltage);

			comHepler.SendInstruction(&Serial, REPLY_STATUS, paraStream);
		}
		return;
	}

	 //if(bIsAutomaticModel)
		// RunAutomatically();
	 //else if(pInstr != NULL)
		 ExecuteInstrunction(cmd);

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

void ExecuteInstrunction(ERxCommand& cmd)
{
	switch (cmd.CommandID())
	{
	case RUN_FORWARD_CMD:
		RunForward();
		break;
	case TURN_LEFT_CMD:
		TurnLeft();
		break;
	case TRUN_RIGHT_CMD:
		TurnRight();
		break;
	case RUN_BACKFARD_CMD:
		RunBackward();
		break;
	default:		
		Stop();
		break;
	}
}

ERINT8 DigitalReadWithCheck(ERUINT8 pin)
{
	// Use read after read to avoid the signal shake.
	ERINT8 waitTime = 2;
	ERINT32 lastStatus2 = digitalRead(pin);
	delay(waitTime);
	ERINT32 lastStatus1 = digitalRead(pin);

	ERINT8 i = 10;
	while(i--)
	{
		delay(waitTime);
		ERINT32 curStatus = digitalRead(pin);
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

ERUINT8 IsHitTheWall()
{
	ERINT8 curStatus = DigitalReadWithCheck(FrontBumperSensorPin);

	return (curStatus != initialSensorStatus);
}


void RunForward()
{
	digitalWrite(M1, 1);
	digitalWrite(M2, 1);

	SetMotorSpeed();

	currentStatus = RUN_FORWARD_CMD;

#if defined(ENABLESTATUSLOG)
	comHepler.SendInstruction(&Serial, 0xFF, "RunForward");
#endif
}


void TurnLeft()
{
	digitalWrite(M1, 0);
	digitalWrite(M2, 1);

	SetMotorSpeed();

	currentStatus = TURN_LEFT_CMD;

#if defined(ENABLESTATUSLOG)
	comHepler.SendInstruction(&Serial, 0xFF, "TurnLeft");
#endif
}

void TurnRight()
{
	digitalWrite(M1, 1);
	digitalWrite(M2, 0);

	SetMotorSpeed();
	currentStatus = TRUN_RIGHT_CMD;

#if defined(ENABLESTATUSLOG)
	comHepler.SendInstruction(&Serial, 0xFF, "TurnRight");
#endif
}

void RunBackward()
{
	digitalWrite(M1, 0);
	digitalWrite(M2, 0);

	SetMotorSpeed();
	currentStatus = RUN_BACKFARD_CMD;

#if defined(ENABLESTATUSLOG)
	comHepler.SendInstruction(&Serial, 0xFF, "RunBackward");
#endif
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
	currentStatus = STOP_CMD;

#if defined(ENABLESTATUSLOG)
	comHepler.SendInstruction(&Serial, 0xFF, "Stop");
#endif
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

void sleepNow()
{
	/* Now is the time to set the sleep mode. In the Atmega8 datasheet
	* http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
	* there is a list of sleep modes which explains which clocks and 
	* wake up sources are available in which sleep modus.
	*
	* In the avr/sleep.h file, the call names of these sleep modus are to be found:
	*
	* The 5 different modes are:
	*     SLEEP_MODE_IDLE         -the least power savings 
	*     SLEEP_MODE_ADC
	*     SLEEP_MODE_PWR_SAVE
	*     SLEEP_MODE_STANDBY
	*     SLEEP_MODE_PWR_DOWN     -the most power savings
	*
	*  the power reduction management <avr/power.h>  is described in 
	*  http://www.nongnu.org/avr-libc/user-manual/group__avr__power.html
	*/  

	//Serial.println("Begin to sleep now.");
#if defined(ENABLESTATUSLOG)
	comHepler.SendInstruction(&Serial, 0xFF, "Begin to sleep now.");
#endif

	set_sleep_mode(SLEEP_MODE_IDLE);   // sleep mode is set here

	sleep_enable();          // enables the sleep bit in the mcucr register
	// so sleep is possible. just a safety pin 

	power_adc_disable();
	power_spi_disable();
	power_timer0_disable();
	power_timer1_disable();
	power_timer2_disable();
	power_twi_disable();


	sleep_mode();            // here the device is actually put to sleep!!

	// THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
	sleep_disable();         // first thing after waking from sleep:
	// disable sleep...

	power_all_enable();

	//Serial.println("Waking up now.");
#if defined(ENABLESTATUSLOG)
	comHepler.SendInstruction(&Serial, 0xFF, "Waking up now.");
#endif
}
