#include "ERxIRCommandRecv.h"

ERxIRCommandRecv::ERxIRCommandRecv(int IRRecvPin) : m_IRRecv(IRRecvPin), m_Results(), m_LastCmd(0)
{
}

void ERxIRCommandRecv::Begin()
{
	m_IRRecv.enableIRIn(); // Start the receiver

}

ERxCommand* ERxIRCommandRecv::GetNextPendingCommand()
{
	if(m_IRRecv.decode(&m_Results))
	{
		m_IRRecv.resume(); // resume receiver

		ERUINT8 newCmdId = 0;

		if(m_Results.decode_type == NEC)
		{
			switch(m_Results.value)
			{
			case REPEAT:
				newCmdId = m_LastCmd;
				break;

			case 0xFD807F: // VOL+
				newCmdId = 0x11; // RunForward
				break;

			case 0xFD906F: // VOL-
				newCmdId = 0x14; // RunBackward
				break;

			case 0xFD20DF: // |<<
				newCmdId = 0x12; // TurnLeft
				break;

			case 0xFD609F: // >>|
				newCmdId = 0x13; // TurnRight
				break;

				// Disable automatic model
			//case 0xFD708F: // ST/REPT
			//	*pNewCmd = 0x01; // Automatic
			//	break;

			case 0xFDB04F: // EQ
				newCmdId = 0x02; // Manual
				break;

			case 0xFD00FF: // Power
				newCmdId = 0x03; // Query
				break;

			default: // 
				newCmdId = 0x10; // Stop
				break;
			}

			m_LastCmd = newCmdId;			
		}
		else
		{
			m_LastCmd = 0;
		}

		ERxCommand* pNewCmd = new ERxCommand(newCmdId);
		return pNewCmd;
	}

	return NULL;	
}

