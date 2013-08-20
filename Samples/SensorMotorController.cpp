#include <ERxCore.h>

/**************************************************************************************
Functionality:	
	<Describe the main functionality of the code in this file.>

The circuit:
	<Describe the used circuit connection.>

More:
	<Add more information regarding the hardware>

Occupied MCU resource:
	<Indicate which resources are occupied by this functionality. 
	When try to implement other functionalities, these resources shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============

/************************************************************************/
//  CMD     PARAMETERS                                          COMMENTS                                                              
//  0x21    ERINT8 ERINT8 ERINT8 ERINT8         CHANGE_SENSOR_TYPE_CMD, index, model(1: input, 0: output), sensor type(1: digital, 0: analog), sensor value(1: high, 0: low)
//  0x22    ERINT32 ...                         RECEIVE_SENSORVALUE_CMD, 16 sensor value (0-1 or 0-1024)
//  0x23    ERINT8 ERINT8 ERINT8  ...           INITIALIZE_SENSORS_CMD, 16 sensor types 
//  0x24                                        REQUEST_SENSORVALUE_CMD  no parameter
//  
//  0x31    ERINT8 ERINT8 ERUINT8               CHANGE_MOTOR_SETTING_CMD, index, type, speed
//  0x32    ERINT8 ....                         INITIALIZE_MOTORS_CMD, 16 the motor types. default speed 127.
//
//  0x41    string                              EXECUTION_RESULT_CMD
/************************************************************************/
#include "Servo.h"
#include "SerialComHelper.h"
#include "ERxProtocol.h"
#include "ERxMotion.h"
#include "ERxSensor.h"

#define CHANGE_SENSOR_TYPE_CMD  0x21
#define RECEIVE_SENSORVALUE_CMD 0x22
#define INITIALIZE_SENSORS_CMD  0x23
#define REQUEST_SENSORVALUE_CMD 0x24

#define CHANGE_MOTOR_SETTING_CMD 0x31
#define INITIALIZE_MOTORS_CMD   0x32

#define EXECUTION_RESULT_CMD	0x41

#define MAX_MOTOR_COUNT		12
#define MAX_SENSOR_COUNT	16

SerialComHelper comHepler;


ERxMotion motorList[MAX_MOTOR_COUNT];
ERxSensor sensorList[MAX_SENSOR_COUNT];

//////////////////////////////////////////////////////////
void setup() {

	// initialization
	Serial.begin(9600);

	for (ERUINT8 i = 0; i < MAX_SENSOR_COUNT; i++)
		sensorList[i].SetData(i, true, true, false);

	for (ERUINT8 i = 0; i < MAX_MOTOR_COUNT; i++)
		motorList[i].SetData(i, true, 127); // default value 127
}

void ResponseResult(const char* pString)
{
	ERINT8* pResult = (ERINT8*)pString;
	ERxParameterStream paraStream;
	paraStream.Write(pResult);

	comHepler.SendInstruction(&Serial, EXECUTION_RESULT_CMD, paraStream);
}

void ReadAndSendSensorValues()
{
	ERxParameterStream paraStream;
	for (ERINT8 i = 0; i < MAX_SENSOR_COUNT; i++)
	{
		paraStream.Write(sensorList[i].GetValue());
	}

	comHepler.SendInstruction(&Serial, RECEIVE_SENSORVALUE_CMD, paraStream);
}

void loop() {

	ERxInstructionFrame* pInstr = comHepler.GetFirstInstruction(&Serial);
	if(pInstr != NULL) 
	{
		ERxParameterStream* pParaStream = pInstr->GetParameterStream();

		switch (pInstr->GetCommand())
		{
		case INITIALIZE_SENSORS_CMD:	
			{
				// set sensor types
				for (ERUINT8 i = 0; i < MAX_SENSOR_COUNT && !pParaStream->Empty(); i++)
				{
					ERINT8 iIsInput = 0;
					pParaStream->Read(&iIsInput);
					ERINT8 iIsDigital = 0;
					pParaStream->Read(&iIsDigital);
					ERINT8 iIsHigh = 0;
					pParaStream->Read(&iIsHigh);
					sensorList[i].SetData(i, iIsInput != 0, iIsDigital != 0, iIsHigh != 0);
				}

				// return sensor values	
				ReadAndSendSensorValues();
				//ResponseResult("OK");
			}
			break;
		case CHANGE_SENSOR_TYPE_CMD: 
			{
				// change the sensor type
				ERINT8 index = 0;
				pParaStream->Read(&index);

				ERINT8 iIsInput = 0;
				pParaStream->Read(&iIsInput);
				ERINT8 iIsDigital = 0;
				pParaStream->Read(&iIsDigital);
				ERINT8 iIsHigh = 0;
				pParaStream->Read(&iIsHigh);

				if(index < MAX_SENSOR_COUNT)
					sensorList[index].SetData(index, iIsInput != 0, iIsDigital != 0, iIsHigh != 0);

				// return sensor values	
				ReadAndSendSensorValues();
				//ResponseResult("OK");
			}
			break;
		case INITIALIZE_MOTORS_CMD: 
			{
				// set motor data.
				for (ERUINT8 i = 0; i < MAX_MOTOR_COUNT && !pParaStream->Empty(); i++)
				{
					ERINT8 iIsMotor = 0;
					pParaStream->Read(&iIsMotor);
					motorList[i].SetData(i, iIsMotor != 0, 127); // default value 127
				}
				//ResponseResult("OK");
			}
			break;
		case CHANGE_MOTOR_SETTING_CMD: 
			{
				// change motor setting.
				ERINT8 index = 0;
				ERINT8 iIsMotor = 0;
				ERUINT8 speed = 127;
				pParaStream->Read(&index);
				pParaStream->Read(&iIsMotor);
				pParaStream->Read(&speed);

				if(index < MAX_MOTOR_COUNT)
				{
					motorList[index].SetData(index, iIsMotor != 0, speed);
					ERxParameterStream paraStream;
					paraStream.Write(index);
					paraStream.Write(iIsMotor);
					paraStream.Write(speed);
					comHepler.SendInstruction(&Serial, EXECUTION_RESULT_CMD, paraStream);
					//ResponseResult("OK");
				}
			}
			break;
		case REQUEST_SENSORVALUE_CMD: 
			{
				ReadAndSendSensorValues();
				//ResponseResult("OK");
			}
			break;

		default:
			ResponseResult("Unknown command");
			break;
		}
	}
}

