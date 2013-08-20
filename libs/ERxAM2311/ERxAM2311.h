/*
ERxAM2311.h - library for Arduino to communicate with AOSONG AM2311. 
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
* This library is used to communicate with AOSONG AM2311 sensor through I2C.
* The sensor data sheet can be found here http://www.aosong.com/cn/products/details.asp?id=93

Required library
* Wire

Project home
* http://code.google.com/p/am2311/

* Created 18 December 2011
* By Jeffrey Sun

*/

#ifndef ERXAM2311_H
#define ERXAM2311_H

#include <Arduino.h>

#define AM2311_LIB_VER "1.0"

class ERxAM2311
{
public:
	int read();
	float humidity;
	float temperature;
};
#endif // ERXAM2311_H
