#pragma once
#ifndef ERX_HOST_H
#define ERX_HOST_H

#include "ERxServiceContext.h"

// Note: This macro defines the max cout of services can be hosted.
// Considering the resource limitation, 
// run 8 services on an Arduino board is enough for most of cases.
// If want more services, enlarge this number.
#define SERVICE_CAPACITY 8

// Uncomment this macro if use external buffer.
//#define USE_INTERNAL_BUFFER

class Print;
class ERxService;

class ERxHost
{
public:

#ifdef USE_INTERNAL_BUFFER
	ERxHost();
#else 
	ERxHost(uint8_t* pCmdParaBuffer, size_t cmdBufferSize
		, uint8_t* pResultBuffer, size_t resultBufferSize);
#endif //USE_INTERNAL_BUFFER

public:
	unsigned int		Run();
	void				Execute(ERxServiceContext& );
	void				PrintHelp(ERxServiceContext& );
	void				HandleResult(ERxServiceContext& );

public:
	bool				AddService(ERxService* );
	ERxServiceContext&	GetServiceContext();

public:
	// Network address
	void				SetMyAddress(unsigned char address);
	unsigned char		GetMyAddress() const;


private:
	bool				Full() const;

private:

	ERxService*			m_services[SERVICE_CAPACITY];
	size_t				m_size;
	ERxServiceContext	m_context;
};

#endif // ERX_HOST_H