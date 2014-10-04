#include "ERxL298NMotorService.h"
#include <ERxL298N.h>
#include <ERxServiceContext.h>

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
	for(int i=0; i<MAX_MOTORS; i++){
		if(!m_leftMotors[i]){
			m_leftMotors[i]->forward();
		}
		if(!m_rightMotors[i]){
			m_rightMotors[i]->forward();
		}
	}
}
void ERxL298NMotorService::backward()
{
	for(int i=0; i<MAX_MOTORS; i++){
		if(!m_leftMotors[i]){
			m_leftMotors[i]->backward();
		}
		if(!m_rightMotors[i]){
			m_rightMotors[i]->backward();
		}
	}
}
void ERxL298NMotorService::turnLeft()
{
	for(int i=0; i<MAX_MOTORS; i++){
		if(!m_leftMotors[i]){
			m_leftMotors[i]->backward();
		}
		if(!m_rightMotors[i]){
			m_rightMotors[i]->forward();
		}
	}
}
void ERxL298NMotorService::turnRight()
{
	for(int i=0; i<MAX_MOTORS; i++){
		if(!m_leftMotors[i]){
			m_leftMotors[i]->forward();
		}
		if(!m_rightMotors[i]){
			m_rightMotors[i]->backward();
		}
	}
}
void ERxL298NMotorService::stop()
{
	for(int i=0; i<MAX_MOTORS; i++){
		if(!m_leftMotors[i]){
			m_leftMotors[i]->stop();
		}
		if(!m_rightMotors[i]){
			m_rightMotors[i]->stop();
		}
	}
}

#define CMD_ROBOT_FORWARD 0x70
#define CMD_ROBOT_BACKWARD 0x71
#define CMD_ROBOT_TURN_RIGHT 0x72
#define CMD_ROBOT_TURN_LEFT 0x73
#define CMD_ROBOT_STOP 0x74

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
			turnRight();
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
