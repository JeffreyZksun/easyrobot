#include "ERxUARTCmdReceiverService.h"
#include "ERxServiceContext.h"
#include "ERxHost.h"
#include "ERxCommandId.h"

// Uncomment the macro to output the debug message.
//#define DEBUG_UART_RECV

#ifdef DEBUG_UART_RECV
#include <HardwareSerial.h>
#endif

ERxUARTCmdReceiverService::ERxUARTCmdReceiverService(Stream *pIORawStream) : m_TextMessage(pIORawStream)
{

}

void ERxUARTCmdReceiverService::Populate(ERxServiceContext& context)
{
	if(m_TextMessage.available())
	{
		// Command Format 
		// <COMMAND_ID>[<SPACE><COMMAND_PARAMETER>]<CR><LF>

		unsigned int cmdId = (unsigned int )m_TextMessage.parseULong();

		if(cmdId != SYS_INVALID_COMMAND)
		{
			context.InvalidateCommand();
#ifdef DEBUG_UART_RECV
			Serial.print("New command: ");
			Serial.print(cmdId);
			Serial.print(CRLF);
#endif

			context.SetCommandId(cmdId);

			ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
			while(m_TextMessage.available())
				rCmdParameterStream.write(m_TextMessage.read());
		}

		m_TextMessage.invalidate();
	}
}

void ERxUARTCmdReceiverService::PrintHelp(ERxServiceContext& context)
{
	ERxOutputStream& rResultStream = context.GetResultStream(); 

	PROG(serviceName) = "=======ERxUARTCmdReceiverService====\r\n";
	rResultStream.printP(serviceName);
}