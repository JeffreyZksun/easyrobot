/*
ERxL298N.h - library for Arduino to control L298N DC motor driver. 
Copyright (c) 2011 Jeffrey Sun.  All right reserved.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
Function
* This library is used to control L298N DC motor driver.
* It only controls to rotation direction. The rotation speed is not controlled.


I/O table of L298N.

E1/2 	M1/2		Motor
1 		1		rotate forward
1 		0		rotate backward
0 		0			stop

The E1/2 pin controls if the motor rotates or not. 1 means rotate. 0 meas stop.
The M1/2 pin controls the rotation direction. 1 and 0 are oposite direction. It only makes sense when E1 is 1.


The circuit:
---------------------------------------------
	Arduino		L298N(Motor Driver)			
	4*				E1
	5*				M1
	6*				E2
	7*				M2

	GND				GND
	VIN(9V)			VD

The pin number with the * suffix means you can change it to be any number.


---------------------------------------------
	Motor Battery	   L298N	 Ardiono
	GND					GND		  GND
	+4.8~35V			VS		  N/A

---------------------------------------------
	Logic Battery	   L298N	 Ardiono
	GND					GND		  GND
	+7~12V				VD	  	  Vin

---------------------------------------------
	L298N		Motor
	M1			DC Motor
	M2			DC Motor
---------------------------------------------

L298N reference: http://item.taobao.com/item.htm?spm=a1z09.2.9.11.fVbF4K&id=227289053&_u=4113ts22070

* Created 4 October 2014
* By Jeffrey Sun

*/

#ifndef ERXL298N_H
#define ERXL298N_H

#include <Arduino.h>

#define L298N_LIB_VER "1.0"

class ERxL298N
{
public:
	/*
	The passed in pins will be set to be OUTPUT in the constructor.

	@constructor
	@param {unsigned char} e the pin index of Arduino which connects to the E1/2 pin of L298N.
	@param {unsigned char} m the pin index of Arduino which connects to the M1/2 pin of L298N.
	@param {bool} reverse indicate if reverse the direction.
	*/
	ERxL298N(unsigned char e, unsigned char m, bool reverse = false);

public:
	void forward();
	void backward();
	void stop();


private:
	unsigned char 	m_e; 
	unsigned char 	m_m;
	bool			m_reverse;
};
#endif // ERXL298N_H
