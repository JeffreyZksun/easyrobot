#include "ERxL298NMotorService.h"
#include <ERxL298N.h>
#include <ERxServiceContext.h>

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

ERxL298NMotorService::ERxL298NMotorService()
{
	// initialize
	for(int i=0; i<MAX_MOTORS; i++){
		m_leftMotors[i]=NULL;
		m_rightMotors[i]=NULL;
	}
}

bool ERxL298NMotorService::addLeftMotor(unsigned char e, unsigned char m, bool reverse/*=false*/){
	for(int i=0; i<MAX_MOTORS; i++){
		if(!m_leftMotors[i]){
			m_leftMotors[i] = new ERxL298N(e, m, reverse);
			DEBUG_PRINT("add left motor: ");
			DEBUG_PRINT_LN(i);
			return true;
		}
	}
	return false;
}

bool ERxL298NMotorService::addRightMotor(unsigned char e, unsigned char m, bool reverse/*=false*/){
	for(int i=0; i<MAX_MOTORS; i++){
		if(!m_rightMotors[i]){
			m_rightMotors[i] = new ERxL298N(e, m, reverse);
			return true;
		}
	}
	return false;
}
void ERxL298NMotorService::forward()
{
	DEBUG_PRINT("Move forward: ");

	for(int i=0; i<MAX_MOTORS; i++){
		if(m_leftMotors[i]){
			DEBUG_PRINT(" LF");DEBUG_PRINT(i);
			m_leftMotors[i]->forward();
		}
		if(m_rightMotors[i]){
			DEBUG_PRINT(" RF");DEBUG_PRINT(i);
			m_rightMotors[i]->forward();
		}
	}

	DEBUG_PRINT_LN(' ');
}
void ERxL298NMotorService::backward()
{
	for(int i=0; i<MAX_MOTORS; i++){
		if(m_leftMotors[i]){
			m_leftMotors[i]->backward();
		}
		if(m_rightMotors[i]){
			m_rightMotors[i]->backward();
		}
	}
}
void ERxL298NMotorService::turnLeft()
{
	DEBUG_PRINT("Turn left: ");
	
	for(int i=0; i<MAX_MOTORS; i++){
		if(m_leftMotors[i]){
			DEBUG_PRINT(" LB");DEBUG_PRINT(i);
			m_leftMotors[i]->backward();
		}
		if(m_rightMotors[i]){
			DEBUG_PRINT(" RF");DEBUG_PRINT(i);
			m_rightMotors[i]->forward();
		}
	}

	DEBUG_PRINT_LN(' ');
}
void ERxL298NMotorService::turnRight()
{
	for(int i=0; i<MAX_MOTORS; i++){
		if(m_leftMotors[i]){
			m_leftMotors[i]->forward();
		}
		if(m_rightMotors[i]){
			m_rightMotors[i]->backward();
		}
	}
}
void ERxL298NMotorService::stop()
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

void ERxL298NMotorService::Execute(ERxServiceContext& context)
{
	bool handled = true;
	/*
	// Command Format 
	<CMD_ROBOT_FORWARD><CR><LF>
	<CMD_ROBOT_BACKWARD><CR><LF>
	<CMD_ROBOT_TURN_RIGHT><CR><LF>
	<CMD_ROBOT_TURN_LEFT><CR><LF>
	<CMD_ROBOT_STOP><CR><LF>
	*/

	switch (context.GetCommandId())
	{
	case CMD_ROBOT_FORWARD: 
		{
			forward();
		}
		break;
	case CMD_ROBOT_BACKWARD: 
		{
			backward();
		}
		break;
	case CMD_ROBOT_TURN_RIGHT: 
		{
			turnRight();
		}
		break;
	case CMD_ROBOT_TURN_LEFT: 
		{
			turnLeft();
		}
		break;
	case CMD_ROBOT_STOP: 
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

void ERxL298NMotorService::PrintHelp(ERxServiceContext& context)
{
	ERxOutputStream& rResultStream = context.GetResultStream();

	PROG(serviceName) = "> ERxL298NMotorService\r\n";
	rResultStream.printP(serviceName);
}
