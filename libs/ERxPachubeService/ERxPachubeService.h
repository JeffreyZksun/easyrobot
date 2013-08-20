/*

This service sends the data to pachube server.The Ethernet should be initialized outside this service. 
Otherwise the service will hang when access the Ethernet.

Required hardware
Arduino
Ethernetshield

Circuit:
* Stack Ethernet shield over the Arduino Board
* Ethernet shield attached to pins 10, 11, 12, 13
* Connect the Ethernet shield to the router


* Created 1 June 2012
* By Jeffrey Sun
* 

*/

#pragma once
#ifndef ERX_TH_DETECTION_SERVICE_H
#define ERX_TH_DETECTION_SERVICE_H


#include "ERxService.h"
#include <ERxPachube.h>

class ERxPachubeService : public ERxService
{
public:
	//ERxPachubeService(
	//	const char* APIKey, unsigned int feedId
	//	, uint8_t* pMac, uint8_t* pIP);

	ERxPachubeService(const char* APIKey, unsigned int feedId);

public:

	virtual void Populate(ERxServiceContext& context);

	virtual void HandleResult(ERxServiceContext& context);

	virtual void PrintHelp(ERxServiceContext& context);

public:
	void			SetSensorAddress(unsigned char address);

private:
	ERxPachubeDataOut	m_dataout;

	unsigned int		m_totalQuantity;
	unsigned int		m_successQuantity;

	bool				m_bIsFirstCommand; // 

	unsigned char		m_sensorAddress; // The address where the DH service is hosted. If <INVALID_ADDRESS>, means the local.
};

#endif // ERX_TH_DETECTION_SERVICE_H