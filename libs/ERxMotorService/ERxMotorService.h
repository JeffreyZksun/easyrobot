/*

This service is for the motor driven robot car. 
There are two sets of motor on the both sides of car.

Required hardware
Arduino
Motor driver (L298N, L9110)

Execute
Executable command format
<CMD_MOTOR><SPACE><ROBOT_FORWARD>[<SPACE><SPEED>]<CR><LF>
<CMD_MOTOR><SPACE><ROBOT_BACKWARD>[<SPACE><SPEED>]<CR><LF>
<CMD_MOTOR><SPACE><ROBOT_TURN_RIGHT>[<SPACE><SPEED>]<CR><LF>
<CMD_MOTOR><SPACE><ROBOT_TURN_LEFT>[<SPACE><SPEED>]<CR><LF>
<CMD_MOTOR><SPACE><ROBOT_STOP><CR><LF>

Result format
<SYS_RESULT> OK<CR><LF>

* Created 4 October 2014
* By Jeffrey Sun
* 

*/
#pragma once
#ifndef ERX_MOTOR_SERVICE_H
#define ERX_MOTOR_SERVICE_H

#include <ERxService.h>

// Define the maximum motors for each set.
#define MAX_MOTORS 2

// 0x70 motor command
#define CMD_MOTOR 0x70


#define ROBOT_FORWARD 0x01
#define ROBOT_BACKWARD 0x02
#define ROBOT_TURN_RIGHT 0x03
#define ROBOT_TURN_LEFT 0x04
#define ROBOT_STOP 0x05

class ERxIMotor;

class ERxMotorService : public ERxService
{
public:
	ERxMotorService();

public:

	// Execute the command saved in the context. The result is output to the context.
	virtual void Execute(ERxServiceContext& context);

	// Print the help message of this service. 
	virtual void PrintHelp(ERxServiceContext& context);

public:
	// Add a motor to the left set of the robot.
	bool addLeftMotor(ERxIMotor* m);
	// Add a motor to the right set of the robot.
	bool addRightMotor(ERxIMotor* m);

private:
	void forward(unsigned int speed = 0);
	void backward(unsigned int speed = 0);
	void turnLeft(unsigned int speed = 0);
	void turnRight(unsigned int speed = 0);
	void stop();

private:
	ERxIMotor *m_leftMotors[MAX_MOTORS];
	ERxIMotor *m_rightMotors[MAX_MOTORS];
};

#endif // ERX_MOTOR_SERVICE_H