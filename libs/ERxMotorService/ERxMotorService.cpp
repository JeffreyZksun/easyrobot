#include "ERxMotorService.h"
#include <ERxIMotor.h>
#include <ERxServiceContext.h>
#include "ERxIOStream.h"

// #define ENABLE_DEBUG_MESSAGE

#ifdef ENABLE_DEBUG_MESSAGE
#include <HardwareSerial.h>

template<class T>
void DEBUG_PRINT_LN(T message) 
{
	Serial.println(message); 
}

template<class T>
void DEBUG_PRINT(T message) 
{
	Serial.print(message); 
}

#else
// define the empty macros
#define DEBUG_PRINT_LN(message)
#define DEBUG_PRINT(message)
#endif

ERxMotorService::ERxMotorService()
{
	// initialize
	for(int i=0; i<MAX_MOTORS; i++){
		m_leftMotors[i]=NULL;
		m_rightMotors[i]=NULL;
	}
}

bool ERxMotorService::addLeftMotor(ERxIMotor* m){
	for(int i=0; i<MAX_MOTORS; i++){
		if(!m_leftMotors[i]){
			m_leftMotors[i] = m;
			DEBUG_PRINT("add left motor: ");
			DEBUG_PRINT_LN(i);
			return true;
		}
	}
	return false;
}

bool ERxMotorService::addRightMotor(ERxIMotor* m){
	for(int i=0; i<MAX_MOTORS; i++){
		if(!m_rightMotors[i]){
			m_rightMotors[i] = m;
			return true;
		}
	}
	return false;
}
void ERxMotorService::forward(unsigned int speed /*= 0*/)
{
	DEBUG_PRINT("Move forward: ");

	for(int i=0; i<MAX_MOTORS; i++){
		if(m_leftMotors[i]){
			DEBUG_PRINT(" LF");DEBUG_PRINT(i);
			m_leftMotors[i]->forward(speed);
		}
		if(m_rightMotors[i]){
			DEBUG_PRINT(" RF");DEBUG_PRINT(i);
			m_rightMotors[i]->forward(speed);
		}
	}

	DEBUG_PRINT_LN(' ');
}
void ERxMotorService::backward(unsigned int speed /*= 0*/)
{
	for(int i=0; i<MAX_MOTORS; i++){
		if(m_leftMotors[i]){
			m_leftMotors[i]->backward(speed);
		}
		if(m_rightMotors[i]){
			m_rightMotors[i]->backward(speed);
		}
	}
}
void ERxMotorService::turnLeft(unsigned int speed /*= 0*/)
{
	DEBUG_PRINT("Turn left: ");
	
	for(int i=0; i<MAX_MOTORS; i++){
		if(m_leftMotors[i]){
			DEBUG_PRINT(" LB");DEBUG_PRINT(i);
			m_leftMotors[i]->backward(speed);
		}
		if(m_rightMotors[i]){
			DEBUG_PRINT(" RF");DEBUG_PRINT(i);
			m_rightMotors[i]->forward(speed);
		}
	}

	DEBUG_PRINT_LN(' ');
}
void ERxMotorService::turnRight(unsigned int speed /*= 0*/)
{
	for(int i=0; i<MAX_MOTORS; i++){
		if(m_leftMotors[i]){
			m_leftMotors[i]->forward(speed);
		}
		if(m_rightMotors[i]){
			m_rightMotors[i]->backward(speed);
		}
	}
}
void ERxMotorService::stop()
{
	for(int i=0; i<MAX_MOTORS; i++){
		if(m_leftMotors[i]){
			m_leftMotors[i]->stop();
		}
		if(m_rightMotors[i]){
			m_rightMotors[i]->stop();
		}
	}
}

void ERxMotorService::Execute(ERxServiceContext& context)
{
	/*
	// Command Format 
	<CMD_MOTOR><SPACE><ROBOT_FORWARD>[<SPACE><SPEED>]<CR><LF>
	<CMD_MOTOR><SPACE><ROBOT_BACKWARD>[<SPACE><SPEED>]<CR><LF>
	<CMD_MOTOR><SPACE><ROBOT_TURN_RIGHT>[<SPACE><SPEED>]<CR><LF>
	<CMD_MOTOR><SPACE><ROBOT_TURN_LEFT>[<SPACE><SPEED>]<CR><LF>
	<CMD_MOTOR><SPACE><ROBOT_STOP><CR><LF>
	*/
	if(context.GetCommandId() == CMD_MOTOR){
		bool handled = true;

		ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
		const unsigned long robotCmd = rCmdParameterStream.parseULong();
		const unsigned int speed = (unsigned int)(rCmdParameterStream.parseULong());

		switch (robotCmd)
		{
		case ROBOT_FORWARD: 
			{
				forward(speed);
			}
			break;
		case ROBOT_BACKWARD: 
			{
				backward(speed);
			}
			break;
		case ROBOT_TURN_RIGHT: 
			{
				turnRight(speed);
			}
			break;
		case ROBOT_TURN_LEFT: 
			{
				turnLeft(speed);
			}
			break;
		case ROBOT_STOP: 
			{
				stop();
			}
			break;
		default:
			handled = false;
			break;
		}

		if(handled){
			ERxOutputStream& rResultStream = context.GetResultStream();
			rResultStream.print("OK");
			rResultStream.print("\r\n");

			context.SetIsCommandExecuted(true);
			context.SetIsCommandSuccess(true);
		}
	}
	
}

void ERxMotorService::PrintHelp(ERxServiceContext& context)
{
	ERxOutputStream& rResultStream = context.GetResultStream();

	PROG(serviceName) = "> ERxMotorService\r\n";
	rResultStream.printP(serviceName);
}
