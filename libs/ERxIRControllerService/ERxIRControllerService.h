/*

This service sends the 38 kHz IR code.

Required library
* IRremote

Required hardware
* Arduino
* IR detector
* IR transmitter 

Circuit:
* Connect IR detector/demodulator to the input recvpin (11).
* Connect IR LED transmitter to the output PWM pin TIMER_PWM_PIN (9) defined in IRremote.h.

Trouble shouting
* The timer 2 (pin3) doesn't work. Test on UNO and duemilanove. The error is very big when send NEC code.So use timer 1 (pin 9) instead.
* Some IR sender hardware can't send out correct signal when the distance becomes far. 


* Created 2 June 2012
* By Jeffrey Sun
* 

*/
#pragma once
#ifndef ERX_IR_CONTROLLER_SERVICE_H
#define ERX_IR_CONTROLLER_SERVICE_H

#include "ERxService.h"
#include <IRremote.h>

class ERxIRControllerService : public ERxService
{
public:
	// Any pint is OK.
	ERxIRControllerService(int recvpin = 11);

public:

	// Execute the command saved in the context. The result is output to the context.
	virtual void Execute(ERxServiceContext& context);

	// Print the help message of this service. 
	virtual void PrintHelp(ERxServiceContext& context);

private:
	void SendIR(unsigned long codeValue, int codeLen, int codeType, bool repeat);

private:
	int			m_recvpin; // Any pin can be used as the receive pin.

	IRsend		m_irsend;
	int			m_toggle; // The RC5/6 toggle state
};

#endif // ERX_IR_CONTROLLER_SERVICE_H