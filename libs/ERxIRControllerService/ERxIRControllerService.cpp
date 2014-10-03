#include "ERxIRControllerService.h"
#include "ERxServiceContext.h"
#include "ERxCommandId.h"

#include <IRremoteInt.h>

// Uncomment the macro to output the debug message.
//#define DEBUG_IR

#ifdef DEBUG_IR
#include <HardwareSerial.h>
#endif

ERxIRControllerService::ERxIRControllerService(int recvpin) : m_recvpin(recvpin)
{

}

void ERxIRControllerService::Execute(ERxServiceContext& context)
{
	// Command Format 
	// <IR_SEND_xxx><SPACE><IR_CODE_VALUE>[<OTHER_PARAMETERS>]

	bool bHandled = true;
	switch (context.GetCommandId())
	{
	case IR_SEND_NEC: 
		{
			const unsigned long codeValue = context.GetCommandParameterStream().parseULong();

#ifdef DEBUG_IR
			Serial.print(codeValue, HEX);
			Serial.print("\r\n");
#endif

			SendIR(codeValue, 32, NEC, false);
		}
		break;
	case IR_SEND_SONY: 
		{
			const unsigned long codeValue = context.GetCommandParameterStream().parseULong();

			SendIR(codeValue, 12, SONY, false);
		}
		break;
	case IR_SEND_RC5: 
		{
			const unsigned long codeValue = context.GetCommandParameterStream().parseULong();

			SendIR(codeValue, 12, RC5, false);
		}
		break;
	case IR_SEND_RC6: 
		{
			const unsigned long codeValue = context.GetCommandParameterStream().parseULong();

			SendIR(codeValue, 20, RC6, false);
		}
		break;
	default:
		bHandled = false;
		break;
	}

	if(bHandled)
	{
		context.SetIsCommandExecuted(true);
		context.SetIsCommandSuccess(true);
		context.GetResultStream().print("OK\r\n");
	}
}

void ERxIRControllerService::PrintHelp(ERxServiceContext& context)
{
	ERxOutputStream& rResultStream = context.GetResultStream();

	PROG(serviceName) = "> ERxIRControllerService\r\n";
	rResultStream.printP(serviceName);
}

void ERxIRControllerService::SendIR(unsigned long codeValue, int codeLen, int codeType, bool repeat)
{
	if (codeType == NEC) {
		if (repeat) {
			m_irsend.sendNEC(REPEAT, codeLen);
		} 
		else {
			m_irsend.sendNEC(codeValue, codeLen);
		}
	} 
	else if (codeType == SONY) {
		m_irsend.sendSony(codeValue, codeLen);
	} 
	else if (codeType == RC5 || codeType == RC6) {
		if (!repeat) {
			// Flip the m_toggle bit for a new button press
			m_toggle = 1 - m_toggle;
		}
		// Put the m_toggle bit into the code to send
		codeValue = codeValue & ~(1 << (codeLen - 1));
		codeValue = codeValue | (m_toggle << (codeLen - 1));
		if (codeType == RC5) {
			m_irsend.sendRC5(codeValue, codeLen);
		} 
		else {
			m_irsend.sendRC6(codeValue, codeLen);
		}
	} 
	else if (codeType == UNKNOWN /* i.e. raw */) {
		// Assume 38 KHz
		// Do nothings, don't send raw
	}
}