#include "ERxCommander.h"


/********************************ERxCommander*****************************/
/*                                                                      */
/************************************************************************/
ERxCommander::ERxCommander()
{
	// Initialize the pointer array.
	for(int i = 0; i < 10; i++)
	{
		m_CmdRecvList[i] = NULL;
	}
}

ERxCommander::~ERxCommander()
{
	CommandBuffer::size_type count =  m_CommandList.Size();
	for(CommandBuffer::size_type i = 0; i < count - 1; i++)
	{
		ERxCommand* pCmd = m_CommandList.Pop();
		delete pCmd;
		pCmd = NULL;
	}
}

void ERxCommander::AppendCommand(ERxCommand* pNewCmd)
{
	m_CommandList.Append(pNewCmd);
}

ERxCommand ERxCommander::GetNextPendingCommand()
{
	UpdateCmdList();

	CommandBuffer::size_type count =  m_CommandList.Size();
	if(0 == count)
	{
		ERxParameterStream dummyPara;
		ERxCommand dummyCmd(0, dummyPara);
		return dummyCmd;
	}
	else
	{
		//for(CommandBuffer::size_type i = 0; i < count - 1; i++)
		//{
		//	m_CommandList.Pop();
		//}

		ERxCommand* pCmd = m_CommandList.Pop();

		ERxCommand tmpCmd = *pCmd;

		delete pCmd;
		pCmd = NULL;

		return tmpCmd;
	}
}

void ERxCommander::UpdateCmdList()
{
	for(int i = 0; i < 10; i++)
	{
		if(m_CmdRecvList[i] != NULL)
		{
			m_CmdRecvList[i]->GetPendingCommands(this);
		}
		else
			return; // Return when the first invalid object is encountered.
	}
}

bool ERxCommander::RegisterCommandReceiver(ERxICommandReceiver* pCmdRecv)
{
	if(NULL == pCmdRecv)
		return false;

	for(int i = 0; i < 10; i++)
	{
		if(m_CmdRecvList[i] == pCmdRecv)
			return true;
		else if(NULL == m_CmdRecvList[i])
		{
			m_CmdRecvList[i] = pCmdRecv;
			return true;
		}
	}

	return false;
}

/*******************************ERxICommandReceiver**********************/
/*                                                                      */
/************************************************************************/

void ERxICommandReceiver::GetPendingCommands(ERxCommander* pCmder)
{
	while(true)
	{
		ERxCommand* pNewCmd = GetNextPendingCommand();
		if (NULL != pNewCmd)
		{
			pCmder->AppendCommand(pNewCmd);
		}
		else
		{
			return;
		}
	}
}

/*******************************ERxCommand*******************************/
/*                                                                      */
/************************************************************************/
ERxCommand::ERxCommand(ERUINT8 cmdID, const ERxParameterStream& parameter)
	: m_CommandID(cmdID), m_Parameter(parameter)
{

}

ERxCommand::ERxCommand(ERUINT8 cmdID)
: m_CommandID(cmdID), m_Parameter()
{

}

ERUINT8 ERxCommand::CommandID() const
{
	return m_CommandID;
}

ERxParameterStream* ERxCommand::Parameter()
{
	return &m_Parameter;
}