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