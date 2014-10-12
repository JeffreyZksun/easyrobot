/**
=Feature=

Receive IR signal and popuate the command.
The NEC, SONY, RC5 and RC6 IR signal types are supported.
The unkown types will be ingored.

@example
	A NEC IR signal would be:
	"0x05 0x67AF NEC\r\n"

=Dependency=

 * Hardwares
  * Arduino resource
     * Serial port (UART)
  * [IR detector http://item.taobao.com/item.htm?spm=a1z10.3.w4002-3141887416.16.Agjmmv&id=7868962921]

 * Softwares
  * Consumed libraries
     * [IRremote http://www.pjrc.com/teensy/td_libs_IRremote.html]

@class ERxIRCmdReceiverService
**/
#pragma once
#ifndef ERX_IR_CMD_RECEIVER_SERVICE_H
#define ERX_IR_CMD_RECEIVER_SERVICE_H

#include "ERxService.h"
#include <IRremote.h>

// define command id
#define IR_CMD 0x06

class ERxIRCmdReceiverService : public ERxService
{
public:
	/**
	@constructor
	@param {int} irInputPin.
	**/
	ERxIRCmdReceiverService(int irInputPin);

public:
	/**
	Read IR signal and populate it as a command.

	The schema of the command is like:
	<IR_CMD><SPACE><IR_VALUE><SPACE><IR_TYPE><CR><LF>
	
	The supported IR_TYPE is NEC, SONY, RC5 or RC6, which is represented by string.
	@example
	"0x05 0x67AF NEC\r\n"

	@method Populate
	**/
	virtual void Populate(ERxServiceContext& context);

	/**
	Show the help message
	@method PrintHelp
	**/
	virtual void PrintHelp(ERxServiceContext& context);

private:
	IRrecv 			m_IRrecv;
	decode_results 	m_Results;
};

#endif // ERX_IR_CMD_RECEIVER_SERVICE_H