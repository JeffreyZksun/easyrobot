#include "ERxIRCmdReceiverService.h"
#include "ERxServiceContext.h"

// Uncomment the macro to output the debug message.
#define DEBUG_IR_RECV

#ifdef DEBUG_IR_RECV
#include <HardwareSerial.h>
#endif

ERxIRCmdReceiverService::ERxIRCmdReceiverService(int irInputPin) 
	: m_IRrecv(irInputPin), m_Results()
{
#ifdef DEBUG_IR_RECV
	Serial.begin(9600);
#endif

	m_IRrecv.enableIRIn(); // Start the receiver
}

void ERxIRCmdReceiverService::Populate(ERxServiceContext& context)
{	
	if (m_IRrecv.decode(&m_Results)) {

		int codeType = m_Results.decode_type;
		char* codeTypeName = "";
		bool hasValidSignal = false;
		
		if (codeType == NEC) {
			codeTypeName = "NEC";
			hasValidSignal = true;

			if (m_Results.value == REPEAT) {
				hasValidSignal = false;

				// Don't record a NEC repeat value as that's useless.
				#ifdef DEBUG_IR_RECV
				Serial.println("NEC repeat; ignoring.");
				#endif
				// return;
			}
		} 
		else if (codeType == SONY) {
			codeTypeName = "SONY";
			hasValidSignal = true;
		} 
		else if (codeType == RC5) {
			codeTypeName = "RC5";
			hasValidSignal = true;
		} 
		else if (codeType == RC6) {
			codeTypeName = "RC6";
			hasValidSignal = true;
		}
		else if (codeType == UNKNOWN) {

			#ifdef DEBUG_IR_RECV
			Serial.println("Received unknown code, raw data:");
			unsigned long codeLen = m_Results.rawlen - 1;
			unsigned int duration = 0;
			// To store raw codes:
			// Drop first value (gap)
			// Convert from ticks to microseconds
			// Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
			for (int i = 1; i <= codeLen; i++) {
				if (i % 2) {
					// m_Results.rawbuf[0] saves the gap, so we start with the second value.
					// Mark
					duration = m_Results.rawbuf[i]*USECPERTICK - MARK_EXCESS; // data unit microsecond
					Serial.print(" m");
				} 
				else {
					// Space
					duration = m_Results.rawbuf[i]*USECPERTICK + MARK_EXCESS;
					Serial.print(" s");
				}
				Serial.print(duration, DEC);
			}
			Serial.print(CRLF);
			#endif
		}
		else {
			#ifdef DEBUG_IR_RECV
			Serial.print("Unexpected codeType: ");
			Serial.println(codeType, DEC);
			#endif
		}

		if(hasValidSignal){
			unsigned long codeValue = m_Results.value;

			#ifdef DEBUG_IR_RECV
			Serial.print("Received ");
			Serial.print(codeTypeName);
			Serial.print(": ");
			Serial.print(codeValue, HEX);
			Serial.print(",bit length: ");
			Serial.println(m_Results.bits);
			
			#endif

			context.InvalidateCommand();
			context.SetCommandId(IR_CMD);
			ERxIOStream& rCmdParameterStream = context.GetCommandParameterStream(); 
			rCmdParameterStream.print(codeValue, HEX);
			rCmdParameterStream.print(' ');
			rCmdParameterStream.print(codeTypeName);
			rCmdParameterStream.print(CRLF);
		}

		m_IRrecv.resume(); // resume receiver
	}
}

void ERxIRCmdReceiverService::PrintHelp(ERxServiceContext& context)
{
	ERxOutputStream& rResultStream = context.GetResultStream(); 

	PROG(serviceName) = "> ERxIRCmdReceiverService\r\n";
	rResultStream.printP(serviceName);
}