/***********************************************************************
Declare the classes:
	ERxIRCommandRecv

ERxIRCommandRecv
	Read data from IR receiver. Analyze the command transferred by the IR.

************************************************************************/
#pragma once
#ifndef ERX_IR_COMMAND_RECV_H
#define ERX_IR_COMMAND_RECV_H

#include <ERxType.h>
#include "ERxCommander.h"
#include <IRremote.h>

class ERxIRCommandRecv: public ERxICommandReceiver
{
public:
	ERxIRCommandRecv(int IRRecvPin);

	void Begin();

protected:
	virtual ERxCommand* GetNextPendingCommand();

private:
	IRrecv			m_IRRecv;
	decode_results	m_Results;
	ERUINT8			m_LastCmd;
};

#endif