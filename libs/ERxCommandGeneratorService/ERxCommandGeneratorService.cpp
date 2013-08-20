#include "ERxCommandGeneratorService.h"
#include "ERxServiceContext.h"
#include "ERxHost.h"
#include "ERxCommandId.h"

ERxCommandGeneratorService::ERxCommandGeneratorService(unsigned int id,const char* pCmdData, Print* pOutputStream)
	: m_cmdId(id), m_pCmdData(pCmdData), m_pOutputStream(pOutputStream)
{

}

void ERxCommandGeneratorService::Populate(ERxServiceContext& context)
{
	context.InvalidateCommand();

	// Command Format 
	// <COMMAND_ID>[<SPACE><COMMAND_PARAMETER>]
	context.SetCommandId(m_cmdId);
	// Issue: There exist defect for the print and write functions for the NULL char pointer. a char is written for NULL.
	// Workaround: Check the pointer prior to writing the data.
	if(m_pCmdData != NULL) 
	{
		ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
		rCmdParameterStream.write(m_pCmdData);
	}
}

void ERxCommandGeneratorService::PrintHelp(ERxServiceContext& context)
{
	ERxOutputStream& rResultStream = context.GetResultStream(); 

	PROG(serviceName) = "=======ERxCommandGeneratorService====\r\n";
	rResultStream.printP(serviceName);
}

void ERxCommandGeneratorService::HandleResult(ERxServiceContext& context)
{

	if (context.GetCommandId() == m_cmdId)
	{
		ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 

		const int paraSize = rCmdParameterStream.available();
		if(m_pOutputStream && paraSize > 0)
		{
			// Output the result to stream.
			while(rCmdParameterStream.available())
			{
				m_pOutputStream->write(rCmdParameterStream.read());
			}
			m_pOutputStream->println();
		}
	}
	else
	{
		// Do nothing
	}
}
