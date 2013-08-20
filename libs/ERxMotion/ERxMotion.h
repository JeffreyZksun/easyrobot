#pragma once
#ifndef ROBOTMOTION_H
#define ROBOTMOTION_H

#include <ERxType.h>
#include <Servo.h>

class ERxMotion
{
public:
	ERxMotion();

	bool SetData(ERUINT8 index, bool bIsMotor, ERUINT8 speed);

private:
	void UpdateSpeed();
	void UpdateMotorSpeed();
	void UpdateServoSpeed();

private:
	ERUINT8 m_Index;
	bool	m_bIsMotor;
	ERUINT8 m_Speed;

	// Servo data
	int SERVO_MIN_DEGREE;
	int SERVO_MAX_DEGREE;
	Servo m_Servo;   // Define the servo
};



#endif // ROBOTMOTION_H