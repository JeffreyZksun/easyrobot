/*
The base class for the 360-degree rotations motors. 
The interface supports the speed control. The recommended speed range is [0, 255]. O is the slowest and 255 is the fastest. If the conceret motor driver doesn't support the speed control, this value will be ingored.

* Created 2 November 2014
* By Jeffrey Sun

*/

#ifndef ERXIMOTOR_H
#define ERXIMOTOR_H

#define IMOTOR_LIB_VER "1.0"

class ERxIMotor
{
public:
	
	virtual ~ERxIMotor(){}

public:
	/**
	@param {unsigned int} speed, the value range is [0, 255]. If the motor supports speed control, will indicate the actual speed. Otherwise, this value will be ignored. And the motor will be running forward in the constant speed.
	**/
	virtual void forward(unsigned int speed = 0){};
	virtual void backward(unsigned int speed = 0){};
	virtual void stop(){};
};
#endif // ERXIMOTOR_H
