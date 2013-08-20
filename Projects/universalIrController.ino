/*
Universal IR Controller.


Requried library
 * ERxTextMessage
 * IRremote

Required hardware
 * Arduino
 * IR detector
 * IR transmitter 
 * LED indicator light

Circuit:
 * An IR detector/demodulator must be connected to the input RECV_PIN (11).
 * An IR LED transmitter must be connected to the output PWM pin TIMER_PWM_PIN (9) defined in IRremote.h.
 * A visible LED indicator light can be connected to STATUS_PIN (13) to provide status.

Trouble shouting
 * The timer 2 (pin3) doesn't work. Test on UNO and duemilanove. The error is very big when send NEC code.So use timer 1 (pin 9) instead.
 * Some IR sender hardware can't send out correct signal when the distance becomes far. 


* Created 6 January 2012
* By Jeffrey Sun

*/
#include <Arduino.h>
#include <ERxTextMessage.h>
#include <IRremote.h>
#include <IRremoteInt.h>

//#define DEBUG // Enable this macro to output debug message.

int RECV_PIN	= 11;
int STATUS_PIN	= 13;

IRrecv irrecv(RECV_PIN);
IRsend irsend;

decode_results results;
bool bIsDecoding = false;
unsigned int rawCodes[RAWBUF]; // The durations if raw
int toggle = 0; // The RC5/6 toggle state

ERxTextMessage textMsg(&Serial);
char MsgBuffer[16];


void PrintHelp();
bool IsEqualCaseInsensitive(const char* pLeft, const char* pRight, unsigned int length);
void DecodeIR();
void SendIR(unsigned long codeValue, int codeLen, int codeType, bool repeat);
void DebugOutput(const char* str);
void DebugOutput(unsigned long codeValue,int base);

void setup()
{
  Serial.begin(9600);
  irrecv.blink13(true);
  pinMode(STATUS_PIN, OUTPUT);


  textMsg.print("Universal IR controller is connected\r\n\r\n");
  PrintHelp();

  bIsDecoding = false;
}

void loop() {

	if(textMsg.available())
	{
		int commandIndex = textMsg.parseInt();
		switch(commandIndex)
		{
		case 0:
			PrintHelp();
			break;
		case 1:
			bIsDecoding = true;
			textMsg.print("250 ok\r\n");
			break;
		case 2:
			bIsDecoding = false;
			textMsg.print("250 ok\r\n");
			break;
		case 3:
			{
				unsigned long codeValue = textMsg.parseULong();

				DebugOutput("Received HEX: ");
				DebugOutput(codeValue, HEX);
				DebugOutput(" DEC: ");
				DebugOutput(codeValue, DEC);
				DebugOutput("\r\n");

				SendIR(codeValue, 32, NEC, false);
				textMsg.print("250 ok\r\n");
			}
			break;
		default:
			textMsg.print("221 error: Unknow command index\r\n");
		}

		textMsg.invalidate();
	}

	if(bIsDecoding)
	{
		irrecv.enableIRIn(); // When send IR, the same timer is used by IRSender to generate PWM.So the interrupt should be enabled again.
		delay(500);
		DecodeIR();
	}
}

void PrintHelp()
{
	textMsg.print("Input the command index\r\n");
	textMsg.print("[0] Help\r\n");
	textMsg.print("[1] Decode begin\r\n");
	textMsg.print("[2] Decode end\r\n");
	textMsg.print("[3] Send NEC (3 0xFFFF)\r\n");
}

bool IsEqualCaseInsensitive(const char* pLeft, const char* pRight, unsigned int length)
{
	char leftStr[length + 1];
	char rightStr[length + 1];
	for (int i = 0; i < length; i++) 
	{
		leftStr[i] = toupper(pLeft[i]);
		rightStr[i] = toupper(pRight[i]);
	}
	leftStr[length] = '\0';
	rightStr[length] = '\0';
	
	return strcmp(leftStr, rightStr) == 0;
}

void DecodeIR()
{
	if (irrecv.decode(&results)) 
	{
		digitalWrite(STATUS_PIN, HIGH);

		// Send out the IR code

		textMsg.print("RECEIVE ");
		int codeType = results.decode_type;
		int count = results.rawlen;
		if (codeType == UNKNOWN) {
			// Message frame: RECEIVE RAW 500m 100s 1000m 500s ....

			int codeLen = results.rawlen - 1;
			// To store raw codes:
			// Drop first value (gap)
			// Convert from ticks to microseconds
			// Tweak marks shorter, and spaces longer to cancel out IR receiver distortion. Why should change it?
			textMsg.print("RAW ");
			for (int i = 1; i <= codeLen; i++) {
			if (i % 2) {
				// Mark
				rawCodes[i - 1] = results.rawbuf[i]*USECPERTICK;// - MARK_EXCESS; 
				} 
			else {
				// Space
				rawCodes[i - 1] = results.rawbuf[i]*USECPERTICK ;//+ MARK_EXCESS;
				}
			Serial.print(rawCodes[i - 1], DEC);

			if (i % 2) // Mark
				textMsg.print("m ");
			else // Space
				textMsg.print("s ");
			}
			textMsg.print("\r\n");
		}
		else {
			if (codeType == NEC) {
				// Message frame: RECEIVE NEC 0xFFFF
				// Message frame: RECEIVE NEC REPEAT 0x0000
				textMsg.print("NEC ");
				if (results.value == REPEAT) {
					textMsg.print("REPEAT ");
				}
			} 
			else if (codeType == SONY) {
				// Message frame: RECEIVE SONY 0xFFFF
				textMsg.print("SONY ");
			} 
			else if (codeType == RC5) {
				// Message frame: RECEIVE RC5 0xFFFF
				textMsg.print("RC5 ");
			} 
			else if (codeType == RC6) {
				// Message frame: RECEIVE RC6 0xFFFF
				textMsg.print("RC6 ");
			} 
			else {
				// Message frame: RECEIVE UNKNOWTYPE 2
				textMsg.print("UNKNOWTYPE-");
				textMsg.print(codeType, DEC);
				textMsg.print(" ");
			}
			textMsg.print("0x");
			textMsg.print(results.value, HEX);
			textMsg.print("\r\n");
		}

		irrecv.resume(); // resume receiver
		digitalWrite(STATUS_PIN, LOW);
	}
}

void SendIR(unsigned long codeValue, int codeLen, int codeType, bool repeat)
{
	digitalWrite(STATUS_PIN, HIGH);

  if (codeType == NEC) {
    if (repeat) {
      irsend.sendNEC(REPEAT, codeLen);
      DebugOutput("Sent NEC repeat\r\n");
    } 
    else {
      irsend.sendNEC(codeValue, codeLen);
      DebugOutput("Sent NEC 0x");
      DebugOutput(codeValue, HEX);
	  DebugOutput("\r\n");
    }
  } 
  else if (codeType == SONY) {
    irsend.sendSony(codeValue, codeLen);
    DebugOutput("Sent Sony ");
    DebugOutput(codeValue, HEX);
	DebugOutput("\r\n");
  } 
  else if (codeType == RC5 || codeType == RC6) {
    if (!repeat) {
      // Flip the toggle bit for a new button press
      toggle = 1 - toggle;
    }
    // Put the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == RC5) {
      DebugOutput("Sent RC5 ");
      DebugOutput(codeValue, HEX);
	  DebugOutput("\r\n");
      irsend.sendRC5(codeValue, codeLen);
    } 
    else {
      irsend.sendRC6(codeValue, codeLen);
      DebugOutput("Sent RC6 ");
      DebugOutput(codeValue, HEX);
	  DebugOutput("\r\n");
    }
  } 
  else if (codeType == UNKNOWN /* i.e. raw */) {
    // Assume 38 KHz
    irsend.sendRaw(rawCodes, codeLen, 38);
    DebugOutput("Sent raw\r\n");
  }

  digitalWrite(STATUS_PIN, LOW);
}

void DebugOutput(const char* str)
{
	Serial.print(str);
}

void DebugOutput(unsigned long codeValue,int base)
{
	Serial.print(codeValue, base);
}
