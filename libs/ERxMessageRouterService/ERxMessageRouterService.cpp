#include "ERxMessageRouterService.h"
#include "ERxCommandId.h"
#include "ERxHost.h"
#include "ERxRedirectOutputStream.h"
#include "ERxIOStream.h"

ERxMessageRouterService::ERxMessageRouterService(ERxHost* pHost, Print* pRedirectPrint, uint8_t* pBuffer, size_t capacity) 
	: m_pHost(pHost), m_status()
	, m_pRedirectPrint(pRedirectPrint), m_pBuffer(pBuffer), m_capacity(capacity)
{
	m_status.bIsRouting = false;
	m_status.bRelayMessage = false;
}

void ERxMessageRouterService::Execute(ERxServiceContext& context)
{
	if(m_status.bIsRouting) // Avoid nested routing.
		return;

	if(0 == m_pHost)
		return;

	m_status.bIsRouting = true;
	switch (context.GetCommandId())
	{
	case MR_SEND_TO: 
		{
			// Command Format 
			// <MR_SEND_TO><SPACE><DEST_NODE_ADDRESS><SPACE><SOURCE_NODE_ADDRESS><SPACE><COMMAND_ID><SPACE><COMMAND_DATA><SPACE><CR><LF>
			ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
			ERxOutputStream& rResultStream = context.GetResultStream(); 

			const unsigned long destAddr = rCmdParameterStream.parseULong();
			const unsigned long myAddr = context.GetMyAddress();

			if(destAddr == myAddr)
			{
				const unsigned long sourceAddr = rCmdParameterStream.parseULong();
				const unsigned long remoteCmdId = rCmdParameterStream.parseULong();

				// Reset the command id and command parameter.

				const int paraSize = rCmdParameterStream.available();
				if(paraSize > 0)
				{
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
				else
				{
					// No parameter

					// Populate new command
					context.InvalidateCommand();
					context.SetCommandId(remoteCmdId);
				}

				// Set the prefix to output stream to redirect the result.

				ERxRedirectOutputStream routerStream(m_pRedirectPrint, m_pBuffer, m_capacity);
				ERxOutputStream* pPreviousStream = context.SetResultStreamOverride(&routerStream);

				{
					// Add prefix to the router stream.
					// Prefix format
					// <MR_SEND_TO><SPACE><SOURCE_NODE_ADDRESS><SPACE><MY_NODE_ADDRESS><SPACE><SYS_RESULT><SPACE><COMMAND_ID><SPACE>
					unsigned char prefixBuffer[40];
					ERxIOStream prefixStream(prefixBuffer, 40, 0);
					prefixStream.print(MR_SEND_TO); // 3 bits
					prefixStream.print(' ');
					prefixStream.print(sourceAddr); // 3
					prefixStream.print(' ');
					prefixStream.print(myAddr);    // 3
					prefixStream.print(' ');
					prefixStream.print(SYS_RESULT); // 3
					prefixStream.print(' ');
					prefixStream.print(remoteCmdId); // 3
					prefixStream.print(' ');

					routerStream.SetPrefix(prefixStream.rdbuf(), prefixStream.available());
				}

				// Execute the wrapped command.
				m_pHost->Execute(context);

				context.SetResultStreamOverride(pPreviousStream); // recover the stream

			}
			else if(m_status.bRelayMessage)
			{
				// The command is NOT for this device. Relay it.
				ERxRedirectOutputStream routerStream(m_pRedirectPrint, m_pBuffer, m_capacity);
				ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
				context.ResetParameterStreamIterator();
				routerStream.print(MR_SEND_TO);
				if(rCmdParameterStream.peek() != ' ')
					routerStream.print(' ');
				routerStream.write(rCmdParameterStream.rdbuf(), rCmdParameterStream.available());
			}

			context.SetIsCommandExecuted(true);
			context.SetIsCommandSuccess(true);

			break;
		}
	default:
		{
			// Do nothing
		}
	}

	m_status.bIsRouting = false;
}

void ERxMessageRouterService::PrintHelp(ERxServiceContext& context)
{
	ERxOutputStream& rResultStream = context.GetResultStream(); 

	PROG(serviceName) = "> ERxMessageRouterService\r\n";
	rResultStream.printP(serviceName);
}

void ERxMessageRouterService::SetRelayMessage(bool b)
{
	m_status.bRelayMessage = b;
}