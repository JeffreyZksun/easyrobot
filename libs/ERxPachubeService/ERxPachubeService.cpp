#include "ERxPachubeService.h"
#include "ERxServiceContext.h"
#include "ERxCommandId.h"

#include <Ethernet.h>
#include <SPI.h>
#include <Time.h>

// Uncomment this macro if want to print debug message.
//#define DEBUG_PACHUBE

#ifdef DEBUG_PACHUBE
#include <HardwareSerial.h>
#endif

#ifdef DEBUG_PACHUBE
void PrintDataStream(const ERxPachube& pachube)
{
	Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++");
	unsigned int count = pachube.countDatastreams();
	Serial.print("data count=> ");
	Serial.println(count);

	Serial.println("<id>,<value>");
	for(unsigned int i = 0; i < count; i++)
	{
		Serial.print(pachube.getIdByIndex(i));
		Serial.print(",");
		Serial.print(pachube.getValueByIndex(i));
		Serial.println();
	}
}
#endif

// Rate-limitation for free account: 5 api requests / minute, Import 500 datapoints / day
// send 1 request at least 1 minutes
#define UPDATE_INTERVAL            1 // minutes
unsigned long last_connect = 0;
//
//ERxPachubeService::ERxPachubeService(
//	const char* APIKey, unsigned int feedId
//	, uint8_t* pMac, uint8_t* pIP) : m_dataout(APIKey, feedId), m_totalQuantity(0), m_successQuantity(0)
//{
//	Ethernet.begin(pMac, pIP);
//
//	m_dataout.addData(0); // Temp-1
//	m_dataout.addData(1); // Hum-1
//	m_dataout.addData(2); // Temp-2
//	m_dataout.addData(3); // Hum-2
//	m_dataout.addData(4); // Success rate
//}


ERxPachubeService::ERxPachubeService(const char* APIKey, unsigned int feedId ) 
	: m_dataout(APIKey, feedId), m_totalQuantity(0), m_successQuantity(0)
{
	//Ethernet.begin(pMac, pIP);

	m_dataout.addData(0); // Temp-1
	m_dataout.addData(1); // Hum-1
	m_dataout.addData(2); // Temp-2
	m_dataout.addData(3); // Hum-2
	m_dataout.addData(4); // Success rate
}

void ERxPachubeService::Populate(ERxServiceContext& context)
{
#ifdef DEBUG_PACHUBE
	//Serial.print("last connect:");
	//Serial.println(last_connect);
	//Serial.print("current:");
	//Serial.println(minute());
#endif

	if (minute() < last_connect) 
		last_connect = minute();

	if (m_bIsFirstCommand || (minute() - last_connect) >= UPDATE_INTERVAL)
	{
		m_bIsFirstCommand = false;
		last_connect = minute();
		
		context.InvalidateCommand();

		if(m_sensorAddress != INVALID_ADDRESS)
		{
			// Command Format 
			// <MR_SEND_TO><SPACE><SENSOR_ADDR><SPACE><MY_ADDR><SPACE><TH_GET><CR><LF>
			context.SetCommandId(MR_SEND_TO);
			ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
			rCmdParameterStream.print(m_sensorAddress); // 3
			rCmdParameterStream.print(' ');
			rCmdParameterStream.print(context.GetMyAddress());    // 3
			rCmdParameterStream.print(' ');
			rCmdParameterStream.print(TH_GET); // 3
			rCmdParameterStream.print("\r\n");
		}
		else
		{		
			// Command Format 
			// <TH_GET><CR><LF>
			context.SetCommandId(TH_GET);
			ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
			rCmdParameterStream.print("\r\n");
		}


#ifdef DEBUG_PACHUBE
		Serial.print("Command id:");
		Serial.println(context.GetCommandId());
#endif
	}
}

void ERxPachubeService::HandleResult(ERxServiceContext& context)
{
	// Command Format 
	// <TH_GET><SPACE><T1><SPACE><H1><SPACE><T2><SPACE><H2><CR><LF>

	switch (context.GetCommandId())
	{
	case TH_GET: 
		{
			m_totalQuantity++;

			ERxIOStream& rParaStream = context.GetCommandParameterStream();
			const float t1 = rParaStream.parseFloat();
			const float h1 = rParaStream.parseFloat();
			const float t2 = rParaStream.parseFloat();
			const float h2 = rParaStream.parseFloat();

			m_dataout.updateData(0, t1);
			m_dataout.updateData(1, h1);
			m_dataout.updateData(2, t2);
			m_dataout.updateData(3, h2);

			char dataBuffer[32];
			sprintf(dataBuffer,"%d/%d", m_successQuantity, m_totalQuantity);
			m_dataout.updateData(4, dataBuffer);

			int status = m_dataout.updatePachube();

			if(200 == status)
				m_successQuantity++;

#ifdef DEBUG_PACHUBE
			Serial.print("sync status code <OK == 200> => ");
			Serial.println(status);

			PrintDataStream(m_dataout);
#endif
		}
		break;
	default:
		break;
	}
}

void ERxPachubeService::PrintHelp(ERxServiceContext& context)
{
	ERxOutputStream& rResultStream = context.GetResultStream();

	PROG(serviceName) = "> ERxPachubeService\r\n";
	rResultStream.printP(serviceName);
}

void ERxPachubeService::SetSensorAddress(unsigned char address)
{
	m_sensorAddress = address;
}