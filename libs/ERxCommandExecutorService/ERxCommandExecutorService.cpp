#include "ERxCommandExecutorService.h"
#include "ERxServiceContext.h"
#include "ERxCommandId.h"

// Uncomment this macro if want to print debug message.
//#define DEBUG_EXE


#ifdef DEBUG_EXE
#include <HardwareSerial.h>
#endif

ERxCommandExecutorService::ERxCommandExecutorService(unsigned int id, const char* pResultData)
	: m_cmdId(id), m_pResultData(pResultData)
{

}

void ERxCommandExecutorService::Execute(ERxServiceContext& context)
{
	if (context.GetCommandId() == m_cmdId)
	{
		// Command Format 
		// <COMMAND_ID>[<SPACE><COMMAND_PARAMETER>]
		ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
		ERxOutputStream& rResultStream = context.GetResultStream();

		// Issue: There exist defect for the print and write functions for the NULL char pointer. a char is written for NULL.
		// Workaround: Check the pointer prior to writing the data.
		if(m_pResultData != NULL)
			rResultStream.write(m_pResultData);

#ifdef DEBUG_EXE
	Serial.print(m_cmdId); Serial.print(' ');
	if(m_pResultData != NULL)
		Serial.write(m_pResultData);

	Serial.print("\r\n");

#endif


		context.SetIsCommandExecuted(true);
		context.SetIsCommandSuccess(true);
	}
	else
	{
		// Do nothing
	}
}

void ERxCommandExecutorService::PrintHelp(ERxServiceContext& context)
{
	ERxOutputStream& rResultStream = context.GetResultStream();

	PROG(serviceName) = "=======ERxCommandExecutorService====\r\n";
	rResultStream.printP(serviceName);
}