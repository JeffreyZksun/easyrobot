#pragma once
#ifndef ERX_SERVICE_H
#define ERX_SERVICE_H

#include "ERxServiceMacro.h"

class ERxServiceContext;

class ERxService
{
public:
	virtual ~ERxService(){}

public:
	// Generate the command based on the current status of the service. 
	// Set it to context if there is. 
	virtual void Populate(ERxServiceContext& context){}

	// Execute the command saved in the context. The result is output to the context.
	virtual void Execute(ERxServiceContext& context){}

	// Print the help message of this service. The result is output to the context.
	virtual void PrintHelp(ERxServiceContext& context){}

	// Response the result. 
	virtual void HandleResult(ERxServiceContext& context){}
};

#endif // ERX_SERVICE_H