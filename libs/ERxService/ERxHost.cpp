
#include "ERxHost.h"
#include "ERxService.h"

#ifdef USE_INTERNAL_BUFFER
#define COMMAND_PARAMETER_BUFFER_SIZE 128
#define RESULT_BUFFER_SIZE 128
static uint8_t sCmdBuffer[COMMAND_PARAMETER_BUFFER_SIZE];
static uint8_t sResultBuffer[RESULT_BUFFER_SIZE];
#endif //USE_INTERNAL_BUFFER

#ifdef USE_INTERNAL_BUFFER
ERxHost::ERxHost() 
	: m_size(0), m_context(sCmdBuffer, COMMAND_PARAMETER_BUFFER_SIZE, sResultBuffer, RESULT_BUFFER_SIZE)
{

}
#else
ERxHost::ERxHost(uint8_t* pCmdParaBuffer, size_t cmdBufferSize
		, uint8_t* pResultBuffer, size_t resultBufferSize)
	: m_size(0), m_context(pCmdParaBuffer, cmdBufferSize, pResultBuffer, resultBufferSize)
{

}
#endif //USE_INTERNAL_BUFFER

bool ERxHost::AddService(ERxService* pService)
{
	if(Full())
		return false;

	if(0 == pService)
		return false;

	m_services[m_size++] = pService;
	return true;
}

unsigned int ERxHost::Run()
{	
	unsigned int count = 0;
	m_context.InvalidateCommand();
	for(size_t i = 0; i < m_size; i++)
	{
		m_services[i]->Populate(m_context);
		if(m_context.HasValidCommand())
		{
			Execute(m_context);

			// No matter the command is executed or not. Invalidate it.
			m_context.InvalidateCommand();
			count++;
		}
	}

	return count;
}

void ERxHost::Execute(ERxServiceContext& context)
{
	for(size_t i = 0; i < m_size; i++)
	{
		context.BeginCommandExecution();
		m_services[i]->Execute(context);
		context.EndCommandExecution(); // Reset the stream for the following services.

		if(context.IsCommandExecuted()) // Only execute the command once.
			break;
	}

	// Handle Result
	if(context.IsCommandExecuted())
	{
		ERxOutputStream& rResultStream = context.GetResultStream(); 
		// Move the data to command parameter
		size_t size = rResultStream.size();

		if(size > 0)
		{
			// Encapsulate the result into the command.
			unsigned int cmdId = context.GetCommandId(); // Cache the command id

			context.InvalidateCommand();
			
			context.SetCommandId(cmdId);
			ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
			uint8_t* pBuffer = rResultStream.rdbuf();
			rCmdParameterStream.write(pBuffer, size); // Set the result as command parameter.

			HandleResult(context);
		}
	}
}

void ERxHost::PrintHelp(ERxServiceContext& context)
{
	for(size_t i = 0; i < m_size; i++)
	{
		m_services[i]->PrintHelp(context);
	}
}

void ERxHost::HandleResult(ERxServiceContext& context)
{
	// The result is encapsulated in the command parameter.
	for(size_t i = 0; i < m_size; i++)
	{
		context.BeginCommandExecution();
		m_services[i]->HandleResult(context);
		context.EndCommandExecution(); // Reset the stream for the following services.
	}
}

ERxServiceContext& ERxHost::GetServiceContext()
{
	return m_context;
}

void ERxHost::SetMyAddress(unsigned char address)
{
	m_context.SetMyAddress(address);
}

unsigned char ERxHost::GetMyAddress() const
{
	return m_context.GetMyAddress();
}

bool ERxHost::Full() const
{
	return (SERVICE_CAPACITY == m_size);
}