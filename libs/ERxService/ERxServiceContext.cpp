#include "ERxServiceContext.h"
#include "ERxServiceMacro.h"


ERxServiceContext::ERxServiceContext(uint8_t* pCmdParaBuffer, size_t cmdBufferSize
									 , uint8_t* pResultBuffer, size_t resultBufferSize) 
	: m_command(pCmdParaBuffer, cmdBufferSize)
	, m_parameterSize(0)
	, m_contextStatus()
	, m_outputStream(pResultBuffer, resultBufferSize)
	, m_pStreamOverride(0)
	, m_myAddress(BROADCAST_ADDRESS)
{
	m_contextStatus.isCommandExecuted = false;
	m_contextStatus.isCommandSuccess = false;
}

void ERxServiceContext::SetCommandId(unsigned int id)
{
	m_command.SetId(id);
}

unsigned int ERxServiceContext::GetCommandId() const
{
	return m_command.GetId();
}

ERxIOStream& ERxServiceContext::GetCommandParameterStream()
{
	return m_command.GetParameterStream();
}


bool ERxServiceContext::HasValidCommand() const
{
	return m_command.IsValid();
}

//ERxServiceCommand& ERxServiceContext::GetCommand()
//{
//	return m_command;
//}

void ERxServiceContext::InvalidateCommand()
{
	m_command.Invalidate();
	m_parameterSize = 0;
}

void ERxServiceContext::BeginCommandExecution()
{
	// Clear the execution result.
	SetIsCommandExecuted(false);
	
	CacheParameterSize();

	GetResultStream().flush();
}

void ERxServiceContext::EndCommandExecution()
{
	// Don't change the execution result in this function.
	ResetParameterStreamIterator();

	// IMPORTANT: Don't flush the result when end the command. It maybe access later.
	// GetResultStream().flush();
}

void ERxServiceContext::CacheParameterSize()
{
	m_parameterSize = (size_t)GetCommandParameterStream().available();
}
;
void ERxServiceContext::ResetParameterStreamIterator()
{
	GetCommandParameterStream().resetIterator(0, m_parameterSize);
}

bool ERxServiceContext::IsCommandExecuted() const
{
	return m_contextStatus.isCommandExecuted;
}

void ERxServiceContext::SetIsCommandExecuted(bool b)
{
	m_contextStatus.isCommandExecuted = b;
}

bool ERxServiceContext::IsCommandSuccess() const
{
	return m_contextStatus.isCommandSuccess;
}

void ERxServiceContext::SetIsCommandSuccess(bool b)
{
	m_contextStatus.isCommandSuccess = b;
}

ERxOutputStream& ERxServiceContext::GetResultStream()
{
	if(m_pStreamOverride != 0)
		return *m_pStreamOverride;

	return m_outputStream;
}

ERxOutputStream* ERxServiceContext::SetResultStreamOverride(ERxOutputStream* pStreamOverride)
{
	ERxOutputStream* pPreviousOverride = m_pStreamOverride;

	if(pStreamOverride == &m_outputStream)
	{
		m_pStreamOverride = NULL;
	}
	else
	{
		m_pStreamOverride = pStreamOverride;
	}

	return pPreviousOverride ? pPreviousOverride : &m_outputStream;
}

void ERxServiceContext::SetMyAddress(unsigned char address)
{
	m_myAddress = address;
}

unsigned char ERxServiceContext::GetMyAddress() const
{
	return m_myAddress;
}