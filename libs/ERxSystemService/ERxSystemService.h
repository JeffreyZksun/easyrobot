#pragma once
#ifndef ERX_SYSTEM_SERVICE_H
#define ERX_SYSTEM_SERVICE_H

#include "ERxService.h"

class ERxHost;

class ERxSystemService : public ERxService
{
public:
	ERxSystemService(ERxHost* pHost);

public:
	// Generate the command based on the current status of the service. 
	// Set it to context if there is. 
	virtual void Populate(ERxServiceContext& context){}

	// Execute the command saved in the command.
	virtual void Execute(ERxServiceContext& context);

	// Print the help message of this service. 
	virtual void PrintHelp(ERxServiceContext& context);

	// Response the result. 
	virtual void HandleResult(ERxServiceContext& context){}

private:
	ERxHost*		m_pHost;
};

#endif // ERX_SYSTEM_SERVICE_H