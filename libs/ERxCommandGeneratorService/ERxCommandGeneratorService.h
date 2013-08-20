/*

This is service for the test purpose. It does the two things below.
 1. Populate a command with the passed in command id and parameter. (Populate)
 2. Output the result data to the output stream. (HandleResult)


* Created 1 June 2012
* By Jeffrey Sun
* 

*/

#pragma once
#ifndef ERX_COMMAND_GENERATOR_SERVICE_H
#define ERX_COMMAND_GENERATOR_SERVICE_H



#include "ERxService.h"

class Print;

class ERxCommandGeneratorService : public ERxService
{
public:
	ERxCommandGeneratorService(unsigned int id, const char* pCmdData, Print* pOutputStream);

public:
	// Generate the command based on the current status of the service. 
	// Set it to context if there is. 
	virtual void Populate(ERxServiceContext& context);

	// Execute the command saved in the command.
	virtual void Execute(ERxServiceContext& context){}

	// Print the help message of this service. 
	virtual void PrintHelp(ERxServiceContext& context);

	// Response the result. 
	virtual void HandleResult(ERxServiceContext& context);

private:
	unsigned int	m_cmdId;     // save command id
	const char*		m_pCmdData;

	Print*			m_pOutputStream;
	
};

#endif // ERX_COMMAND_GENERATOR_SERVICE_H