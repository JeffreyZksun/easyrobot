#include "ERxUARTCommandRecv.h"

ERxUARTCommandRecv::ERxUARTCommandRecv(HardwareSerial* pSerial) : m_pSerial(pSerial)
{
}

ERxCommand* ERxUARTCommandRecv::GetNextPendingCommand()
{
	ERxInstructionFrame* pInstr = m_ComHepler.GetFirstInstruction(m_pSerial);
	if(pInstr != NULL) 
	{
		ERxCommand* pNewCmd = new ERxCommand(pInstr->GetCommand(), *(pInstr->GetParameterStream()));
		return pNewCmd;
	}

	return NULL;	
}


