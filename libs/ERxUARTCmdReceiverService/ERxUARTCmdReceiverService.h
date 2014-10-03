/**
=Feature=

Receive the text message from the stream which is a UART port generally,
 and populate it as the command.

The command is based on the text message protocol. The whole command is transferred
by ASCII chars. 

@example
The two messages below indicate the same command:

	"0x0A 23 abc\r\n"
	"10 23 abc\r\n"

The data process workflow via UART is:

<incoming data> --> UART --> ERxTextMessage --> ERxUARTCmdReceiverService

=Dependency=

 * Hardwares
  * Arduino resource
     * Serial port (UART)

 * Softwares
  * Consumed libraries
     * ERxTextMessage

@class ERxUARTCmdReceiverService
**/
#pragma once
#ifndef ERX_UART_CMD_RECEIVER_SERVICE_H
#define ERX_UART_CMD_RECEIVER_SERVICE_H

#include "ERxService.h"
#include <ERxTextMessage.h>

class ERxUARTCmdReceiverService : public ERxService
{
public:
	/**
	@constructor
	@param {Stream} pIORawStream this is the UART port generally.
	**/
	ERxUARTCmdReceiverService(Stream *pIORawStream);

public:
	/**
	Read the incoming data from stream and generate the new command to context.

	The schema of the command is like:
	<CMD_ID> <CMD_DATA><CR><LF>

	@method Populate
	**/
	virtual void Populate(ERxServiceContext& context);

	/**
	Show the help message
	@method PrintHelp
	**/
	virtual void PrintHelp(ERxServiceContext& context);

private:
	ERxTextMessage		m_TextMessage;
};

#endif // ERX_UART_CMD_RECEIVER_SERVICE_H