/************************************************************************/
// Servo
//	Futaba S3003 (www.futaba.com Japan), rotation angle [-90, +90], PWM width [ ,2300]
//	SH (Ì¨ÍåÙ¥ºê)
/************************************************************************/
#include <ERxMotion.h>

// BEGIN TEST
//#include <SerialComHelper.h>
//#include <HardwareSerial.h>
// END TEST

ERxMotion::ERxMotion():m_Index(0), m_bIsMotor(true), m_Speed(127)
, SERVO_MIN_DEGREE(700)// -90 degrees
, SERVO_MAX_DEGREE(2300)// +90 degrees
{

}

bool ERxMotion::SetData(ERUINT8 index, bool bIsMotor, ERUINT8 speed)
{
	// change the data
	m_Index = index;
	m_bIsMotor = bIsMotor;
	m_Speed = speed;

	UpdateSpeed();

	return true;
}

void ERxMotion::UpdateSpeed()
{
	if(m_Index < 2) // Index 0, 1 are for UART
		return;

	if(m_bIsMotor)
	{
		UpdateMotorSpeed();
	}
	else
	{
		UpdateServoSpeed();
	}
}

void ERxMotion::UpdateMotorSpeed()
{

}

void ERxMotion::UpdateServoSpeed()
{
	// Note the usage of index.
	m_Servo.attach(m_Index, SERVO_MIN_DEGREE, SERVO_MAX_DEGREE);	
	float speed = m_Speed;
	int pos = SERVO_MIN_DEGREE + (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) * (speed / 255);
	m_Servo.writeMicroseconds(pos);

	// For test
	//SerialComHelper comHelper;
	//ERUINT8 cmd = 0xF1;
	//ERxParameterStream paraStream;
	//paraStream.Write(m_Index);
	//paraStream.Write((ERINT32)SERVO_MIN_DEGREE);
	//paraStream.Write((ERINT32)SERVO_MAX_DEGREE);
	//paraStream.Write((ERINT32)pos);
	//comHelper.SendInstruction(&Serial, cmd, paraStream);
}