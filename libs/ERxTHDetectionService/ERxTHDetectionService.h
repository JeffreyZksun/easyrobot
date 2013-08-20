/*

This service gets the temperature and humidity data from am2311 and dht11 sensors.

Required hardware
Arduino
Sensor DHT11
Sensor AM2311

Circuit:
* Connect the data pin of DHT11 to Arduino pin dht11pin. (The user can select any pin.)
* Connect the SDA and SCL pins of AM2311 to SDA and SCL pins of Arduino
* Connect the VCC and GND pins of DHT11 and AM2311 to +5 and GND pins of Arduino


* Created 1 June 2012
* By Jeffrey Sun
* 

*/
#pragma once
#ifndef ERX_TH_DETECTION_SERVICE_H
#define ERX_TH_DETECTION_SERVICE_H

#include "ERxService.h"

class ERxTHDetectionService : public ERxService
{
public:
	// Any pint is OK.
	ERxTHDetectionService(int dht11pin = 2);

public:

	// Execute the command saved in the context. The result is output to the context.
	virtual void Execute(ERxServiceContext& context);

	// Print the help message of this service. 
	virtual void PrintHelp(ERxServiceContext& context);

private:
	int		m_dht11pin;
};

#endif // ERX_TH_DETECTION_SERVICE_H