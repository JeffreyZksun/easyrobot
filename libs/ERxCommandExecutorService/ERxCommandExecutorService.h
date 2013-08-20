/*

This is service for the test purpose. It does the one below.
 1. Execute the specified command and set the passed in data as the output result. (Execute)


* Created 1 June 2012
* By Jeffrey Sun
* 

*/
#pragma once
#ifndef ERX_COMMAND_EXECUTOR_SERVICE_H
#define ERX_COMMAND_EXECUTOR_SERVICE_H

// This is service for the test purpose. It executes the specified command and outputs the parameter as result.

#include "ERxService.h"

class ERxCommandExecutorService : public ERxService
{
public:
	ERxCommandExecutorService(unsigned int id, const char* pResultData);

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
	const unsigned int	m_cmdId;     // save command id
	const char*			m_pResultData; // the result
	
};

#endif // ERX_COMMAND_EXECUTOR_SERVICE_H