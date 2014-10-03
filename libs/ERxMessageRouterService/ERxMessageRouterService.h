/**
 = Features =
 * If the message is to the current node, delegate the host to execute the command.
 * Only one level of the router prefix is supported. If the beginning of the {{{<command id and command data>}}} is {{{<MR_SEND_TO>}}}, which is a router command, this command will be discarded but tagged as executed. No further execution is made.
 * If the message isn't to the current node, find a path to send out.
 * Set the context correctly to make sure the command result can be sent to the correct stream.

 = Populate =
 N/A

 = Execute =

 * Executable command format
  <MR_SEND_TO> <DEST_NODE_ADDRESS> <SOURCE_NODE_ADDRESS> <command id and command data> <CR><LF>

 * Result format
 N/A

@class ERxMessageRouterService
**/
#pragma once
#ifndef ERX_MESSAGE_ROUTER_SERVICE_H
#define ERX_MESSAGE_ROUTER_SERVICE_H

#include "ERxService.h"
#include "Print.h"

class ERxHost;

class ERxMessageRouterService : public ERxService
{
public:
	ERxMessageRouterService(ERxHost* pHost, Print* pRedirectPrint, uint8_t* pBuffer, size_t capacity);

public:
	// Execute the command saved in the context. The result is output to the context.
	virtual void Execute(ERxServiceContext& context);

	// Print the help message of this service. 
	virtual void PrintHelp(ERxServiceContext& context);

public:
	void		SetRelayMessage(bool b);

private:
	typedef struct 
	{
		unsigned char bIsRouting: 1;
		unsigned char bRelayMessage: 1;
	} StatusFlag;

private:

	ERxHost*		m_pHost;
	StatusFlag		m_status;

	Print*			m_pRedirectPrint;
	uint8_t* 		m_pBuffer;
	size_t 			m_capacity;

};

#endif // ERX_MESSAGE_ROUTER_SERVICE_H