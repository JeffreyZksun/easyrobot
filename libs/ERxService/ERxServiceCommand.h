#pragma once
#ifndef ERX_SERVICE_COMMAND_H
#define ERX_SERVICE_COMMAND_H

#include <ERxIOStream.h>

class ERxServiceCommand
{
public:
	ERxServiceCommand(uint8_t* pCmdParaBuffer, size_t cmdBufferSize);

public:
	void			SetId(unsigned int id);
	unsigned int	GetId() const;
	bool			IsValid() const;

	void			Invalidate();

public:
	ERxIOStream&	GetParameterStream();

private:
	unsigned int	m_cmdId;		// Save command id
	ERxIOStream		m_paraStream;	// Save command parameters

};

#endif // ERX_SERVICE_COMMAND_H