#include "ERxTHDetectionService.h"
#include "ERxServiceContext.h"
#include "ERxCommandId.h"

#include <ERxAM2311.h>
#include <Wire.h>
#include <DHT11.h>

// Uncomment the macro to output the debug message.
//#define DEBUG_TH

#ifdef DEBUG_TH
#include <HardwareSerial.h>
#endif

ERxTHDetectionService::ERxTHDetectionService(int dht11pin) : m_dht11pin(dht11pin)
{

}

void ERxTHDetectionService::Execute(ERxServiceContext& context)
{
	// Command Format 
	// <TH_GET>[<SPACE><COMMAND_PARAMETER>]

	switch (context.GetCommandId())
	{
	case TH_GET: 
		{
			ERxAM2311 amSensor;
			dht11 dhtSensor;

			amSensor.read();
			dhtSensor.read(m_dht11pin);

			ERxOutputStream& rResultStream = context.GetResultStream();
			rResultStream.print(amSensor.temperature, 2); // float
			rResultStream.print(' ');
			rResultStream.print(amSensor.humidity, 2); // float
			rResultStream.print(' ');
			rResultStream.print(dhtSensor.temperature); // int
			rResultStream.print(' ');
			rResultStream.print(dhtSensor.humidity); // int
			rResultStream.print("\r\n");

			context.SetIsCommandExecuted(true);
			context.SetIsCommandSuccess(true);

#ifdef DEBUG_TH
			Serial.print("TH OK");
			Serial.print("\r\n");
#endif
		}
		break;
	default:
		break;
	}
}

void ERxTHDetectionService::PrintHelp(ERxServiceContext& context)
{
	ERxOutputStream& rResultStream = context.GetResultStream();

	PROG(serviceName) = "> ERxTHDetectionService\r\n";
	rResultStream.printP(serviceName);
}