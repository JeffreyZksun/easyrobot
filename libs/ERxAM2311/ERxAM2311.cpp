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

#include "ERxAM2311.h"
#include <Wire.h>
#include <HardwareSerial.h>

// Enable this macro to output the debug message.
//#define OUTPUT_DEBUG_MESSAGE

// The slave address is defined in the data sheet of the sensor.
// It the data after left shift with one bit. 
// In the implementation of Wire library, it suppose the passed in slave address isn't shift.
// So when pass the address to Wire library, the used address should be 0xB8 >> 1.
#define AM2311_ADDR 0xB8 
uint8_t gSlaveAddr = AM2311_ADDR >> 1;

bool bInitialized = false;

// return values:
//  0 : OK
// -1 : checksum error
// -2 : less data
int ERxAM2311::read()
{
#ifdef OUTPUT_DEBUG_MESSAGE
	Serial.println("============Begin================");
#endif
	if(!bInitialized)
	{
#ifdef OUTPUT_DEBUG_MESSAGE
		Serial.println("Initializing");
#endif
		Wire.begin();
		bInitialized = true;
	}

	// Waken
	Wire.beginTransmission(gSlaveAddr);
	uint8_t iRet = Wire.endTransmission();	

#ifdef OUTPUT_DEBUG_MESSAGE
	Serial.print("Wake sensor result: ");
	Serial.println(iRet);
#endif
	/*
		The data format send to the sensor
		+------+---------+-------------+--------------+
		|TYPE  |FUN CODE | START ADDR  | DATA LENGTH  |
		+------+---------+-------------+--------------+
		|LENGTH|   1     |       1     |   1          |
		+------+---------+-------------+--------------+
		|EXAM  |  0x03   |     0x00    |   0x04       |
		+------+---------+-------------+--------------+
	*/
	// write data
	uint8_t dataBuffer[3] = {0x03, 0x00, 0x04};	
	Wire.beginTransmission(gSlaveAddr);
	Wire.write(dataBuffer,3);
	uint8_t iRet2 = Wire.endTransmission();	

#ifdef OUTPUT_DEBUG_MESSAGE
	Serial.print("Send data to slave result: ");
	Serial.println(iRet2);
#endif

	delay(2); // Per the data sheet, at least wait 1.5 ms.

	// Read data
	//Wire.beginTransmission(w);
	/*
		The data format returned form the sensor
		+------+---------+-------------+--------------+--------------+--------------+--------------+----------+
		|TYPE  |FUN CODE | DATA LENGTH | REG1 (HUM-H) | REG2 (HUM-L) | REG3 (TEM-H) | REG4 (YEM-H) | CRC (HL) | 
		+------+---------+-------------+--------------+--------------+--------------+--------------+----------+
		|LENGTH|   1     |       1     |   1          |     1        |      1       |      1       | 2        | 
		+------+---------+-------------+--------------+--------------+--------------+--------------+----------+
		|EXAM  |  0x03   |     0x04    |   0x01       |     0xF4     |      0x00    |      0xFA    | 315A     | 
		+------+---------+-------------+--------------+--------------+--------------+--------------+----------+
	*/
	uint8_t expectedDataLen = 8;
	uint8_t len = Wire.requestFrom(gSlaveAddr, expectedDataLen); 

#ifdef OUTPUT_DEBUG_MESSAGE
	Serial.print("Received data length: ");
	Serial.println(len);
#endif

	if(Wire.available() < expectedDataLen)
	{
#ifdef OUTPUT_DEBUG_MESSAGE
		Serial.print("Error: Less data than expect(8)");
		Serial.println(len);
		Serial.println("============End================");
		return -2;
#endif
	}

	uint8_t funCode = Wire.read();
	uint8_t dataLen = Wire.read();
	uint8_t humH = Wire.read();
	uint8_t humL = Wire.read();
	uint8_t TempH = Wire.read();
	uint8_t TempL = Wire.read();
	uint8_t CRCH = Wire.read();
	uint8_t CRCL = Wire.read();

#ifdef OUTPUT_DEBUG_MESSAGE
	Serial.print("funCode: ");
	Serial.println(funCode);

	Serial.print("dataLen: ");
	Serial.println(dataLen);

	Serial.print("humH: ");
	Serial.println(humH);

	Serial.print("humL: ");
	Serial.println(humL);

	Serial.print("TempH: ");
	Serial.println(TempH);

	Serial.print("TempL: ");
	Serial.println(TempL);

	Serial.print("CRC: ");
	Serial.print(CRCH, HEX);
	Serial.println(CRCL, HEX);
#endif
	
	// Calculate the humidity and temperature per the data sheet.
	humidity = humH * 256;
	humidity += humL;
	humidity /= 10;
	temperature = TempH * 256;
	temperature += TempL;
	temperature /= 10;

	// ToDo - check CRC

#ifdef OUTPUT_DEBUG_MESSAGE
	Serial.println("============End================");
#endif
	return 0;
}

