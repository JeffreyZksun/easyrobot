#pragma once
#ifndef ERX_UART_CMD_RECEIVER_SERVICE_H
#define ERX_UART_CMD_RECEIVER_SERVICE_H

#include "ERxService.h"
#include <ERxTextMessage.h>

class ERxUARTCmdReceiverService : public ERxService
{
public:
	ERxUARTCmdReceiverService(Stream *pIORawStream);

public:
	// Generate the command based on the current status of the service. 
	// Set it to context if there is. 
	virtual void Populate(ERxServiceContext& context);

	// Print the help message of this service. 
	virtual void PrintHelp(ERxServiceContext& context);

private:
	ERxTextMessage		m_TextMessage;
};

#endif // ERX_UART_CMD_RECEIVER_SERVICE_H