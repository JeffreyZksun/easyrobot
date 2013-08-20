/***********************************************************************
Declare the classes:
	ERxUARTCommandRecv

ERxUARTCommandRecv
	Read data from UART. Analyze the command transferred by the UART.

************************************************************************/
#pragma once
#ifndef ERX_UART_COMMAND_RECV_H
#define ERX_UART_COMMAND_RECV_H

#include <ERxType.h>
#include "ERxCommander.h"
#include "SerialComHelper.h"

class HardwareSerial;

class ERxUARTCommandRecv: public ERxICommandReceiver
{
public:
	ERxUARTCommandRecv(HardwareSerial* pSerial);

protected:
	virtual ERxCommand* GetNextPendingCommand();

private:
	SerialComHelper m_ComHepler;
	HardwareSerial* m_pSerial;
};

#endif