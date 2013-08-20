/***********************************************************************
Declare the classes:
	ERxCommander
	ERxICommandReceiver
	ERxCommand

ERxCommander
	It is designed as a command manager.
	It caches all the received and un-executed commands.
	It calls the command receiver to get the pending commands.

ERxICommandReceiver
	The base class for the command receiver. The derived class defined the behavior how 
	to get the command.

ERxCommand
	Encapsulates the command id and the command paramenter

************************************************************************/
#pragma once
#ifndef ERXCOMMANDER_H
#define ERXCOMMANDER_H

#include "ERxRingBuffer.h"
#include "ERxProtocol.h"
#if !defined(PC_PROCESSOR) // For MCU only
	#include "ERxNew.h"
#endif

class ERxICommandReceiver;
class ERxCommand;

class ERxCommander
{
public:
	friend class ERxICommandReceiver;

public:
	ERxCommander();
	~ERxCommander();

	// Get the next pending command. The returned command will be removed from the list.
	ERxCommand	GetNextPendingCommand();
	bool		RegisterCommandReceiver(ERxICommandReceiver* pCmdRecv);

private:
	// Loop all the registered receivers to get their pending commands and add to the list.
	void		UpdateCmdList();
	
	// Add a command to the cached command list.
	void		AppendCommand(ERxCommand* pNewCmd);


private:
	typedef ERxRingBuffer<ERxCommand*, 50> CommandBuffer;

	CommandBuffer			m_CommandList;	// Cache all the commands. ToDo - We should save the command parameter.
	ERxICommandReceiver*	m_CmdRecvList[10]; // They should be release outside this class.
};

class ERxICommandReceiver
{
public:
	friend class ERxCommander;

private:
	// Add all the pending commands to the commander.
	void			GetPendingCommands(ERxCommander* pCmder);

protected:
	virtual ERxCommand*	GetNextPendingCommand() = 0;
};

class ERxCommand
{
public:
	ERxCommand(ERUINT8 cmdID, const ERxParameterStream& parameter);
	ERxCommand(ERUINT8 cmdID);
	// The default implementation(bitwise copy) for copy constructor and = operator is enough for us
	//ERxCommand(const ERxCommand&);
	//ERxCommand& operator=(const ERxCommand&);

	ERUINT8					CommandID() const;

	// Read/write operation will change the cursor of the parameter.
	ERxParameterStream*		Parameter();

private:
	ERUINT8				m_CommandID;	
	ERxParameterStream	m_Parameter;

};
#endif

