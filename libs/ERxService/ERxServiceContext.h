#pragma once
#ifndef ERX_SERVICE_CONTEXT_H
#define ERX_SERVICE_CONTEXT_H

#include "ERxServiceCommand.h"
#include "ERxBufferOutputStream.h"
#include "ERxIOStream.h"

class ERxServiceContext
{
public:
	ERxServiceContext(uint8_t* pCmdParaBuffer, size_t cmdBufferSize
		, uint8_t* pResultBuffer, size_t resultBufferSize);

public:
	// Command status
	void				SetCommandId(unsigned int id);
	unsigned int		GetCommandId() const;
	ERxIOStream&		GetCommandParameterStream();

	bool				HasValidCommand() const;
	void				InvalidateCommand();

	// Command execution
	void				BeginCommandExecution();
	void				EndCommandExecution();

	void				ResetParameterStreamIterator();

public:
	// Command execution result
	bool				IsCommandExecuted() const;
	void				SetIsCommandExecuted(bool);

	bool				IsCommandSuccess() const;
	void				SetIsCommandSuccess(bool);

public:
	// Network address
	void				SetMyAddress(unsigned char address);
	unsigned char		GetMyAddress() const;

public:
	ERxOutputStream&	GetResultStream();
	ERxOutputStream*	SetResultStreamOverride(ERxOutputStream* pStreamOverride);


private:
	void				CacheParameterSize();

private:
	typedef struct 
	{
		unsigned char isCommandExecuted: 1;
		unsigned char isCommandSuccess: 1;
	} StatusFlag;

private:

	ERxServiceCommand	m_command;
	//ERxIOStream			m_cmdParameterStream;
	size_t				m_parameterSize;

	// Result
	StatusFlag			m_contextStatus;
	ERxBufferOutputStream	m_outputStream; // Cache the output in the memory
	ERxOutputStream*	m_pStreamOverride; // override the local stream to redict the result.

	// Network
	unsigned char		m_myAddress; // The logic address of this host.
};

#endif // ERX_SERVICE_CONTEXT_H