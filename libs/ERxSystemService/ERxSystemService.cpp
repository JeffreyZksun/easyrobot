#include "ERxSystemService.h"
#include "ERxServiceContext.h"
#include "ERxHost.h"
#include "ERxCommandId.h"

ERxSystemService::ERxSystemService(ERxHost* pHost) : m_pHost(pHost)
{

}

void ERxSystemService::Execute(ERxServiceContext& context)
{
	if(0 == m_pHost)
		return;

	switch(context.GetCommandId())
	{
	case SYS_HELP:
		// Command Format 
		// <SYS_HELP>[<SPACE><COMMAND_PARAMETER>]
		m_pHost-> PrintHelp( context);
		context.SetIsCommandExecuted(true);
		context.SetIsCommandSuccess(true);
		break;
	case SYS_RESULT:
		// Dispatch the result from the remote device.

		// Command Format 
		// <SYS_HELP>[<SPACE><COMMAND_PARAMETER>]

		{
			// Populate the command for result
			ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
			const unsigned long remoteCmdId = rCmdParameterStream.parseULong();

			if(SYS_INVALID_COMMAND == remoteCmdId)
				break;

			// Reset the command id and command parameter.

			const int paraSize = rCmdParameterStream.available();
			if(paraSize == 0)
				break;  // Don't dispatch the result if there is no result data.

			unsigned char paraBuffer[paraSize];

			int i = 0;
			while(rCmdParameterStream.available() && i < paraSize)
			{
				paraBuffer[i] = (rCmdParameterStream.read());
				i++;
			}

			// Populate new command
			context.InvalidateCommand();
			context.SetCommandId(remoteCmdId);
			rCmdParameterStream.write(paraBuffer, paraSize);
		}

		m_pHost-> HandleResult( context);
		context.SetIsCommandExecuted(true);
		context.SetIsCommandSuccess(true);
		break;
	default:
		// Do nothing
		break;
	}
}

void ERxSystemService::PrintHelp(ERxServiceContext& context)
{

	ERxOutputStream& rResultStream = context.GetResultStream();

	PROG(serviceName) = "> ERxSystemService\r\n";
	rResultStream.printP(serviceName);
}