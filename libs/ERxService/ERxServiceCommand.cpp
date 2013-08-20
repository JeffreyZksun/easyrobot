#include "ERxServiceCommand.h"

#include "ERxCommandId.h"
#include "ERxIOStream.h"

ERxServiceCommand::ERxServiceCommand(uint8_t* pCmdParaBuffer, size_t cmdBufferSize) 
	: m_cmdId(SYS_INVALID_COMMAND)
	, m_paraStream(pCmdParaBuffer, cmdBufferSize, 0)
{

}

void ERxServiceCommand::SetId(unsigned int id)
{
	m_cmdId = id;
}

unsigned int ERxServiceCommand::GetId() const
{
	return m_cmdId;
}

bool ERxServiceCommand::IsValid() const
{
	return m_cmdId != SYS_INVALID_COMMAND;
}

void ERxServiceCommand::Invalidate()
{
	m_cmdId = SYS_INVALID_COMMAND;
	m_paraStream.flush(); // Discard all the data.
}

ERxIOStream& ERxServiceCommand::GetParameterStream()
{
	return m_paraStream;
}