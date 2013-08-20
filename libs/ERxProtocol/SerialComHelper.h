#ifndef SERIALCOMHELPER_H
#define SERIALCOMHELPER_H
/*******************************************************

This class is used to send and receive the instruction frame.
It defines some friendly interfaces to simplify the usage of the protocol.

Examples;
Serial.begin(9600);
SerialComHelper::SendInstruction(&Serial, 1,"OK");
SerialComHelper::SendInstruction(&Serial, 2,(ERUINT8)(0xAA));
SerialComHelper::SendInstruction(&Serial, 3,(char)(0xF1));
SerialComHelper::SendInstruction(&Serial, 5,(long)(0xFF0011));
SerialComHelper::SendInstruction(&Serial, 6,(float)(12.5));

********************************************************/
#include <ERxProtocol.h>

class HardwareSerial; // The base class to read from and write the data to the target device.

class SerialComHelper
{
public:

	void SendInstruction(HardwareSerial* pSerial, ERUINT8 sourceDevice, ERUINT8  targetDevice
		,ERUINT8 cmd, const ERxParameterStream& parameterStream);

	void SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
		, const ERxParameterStream& parameterStream);

	ERxInstructionFrame* GetFirstInstruction(HardwareSerial* pSerial);

public:
	void SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
		,const ERINT8[]);
	void SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
		,const char[]);
	void SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
		,ERINT8);
	void SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
		,ERUINT8);
	void SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
		,ERINT32);
	void SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
		,ERFLOAT32);

public:
	ERUINT8 ReadParameterAsUChar(ERxInstructionFrame* pInstFrame);
	ERINT8 ReadParameterAsChar(ERxInstructionFrame* pInstFrame);
	ERINT32 ReadParameterAsLong(ERxInstructionFrame* pInstFrame);
	ERFLOAT32 ReadParameterAsFloat(ERxInstructionFrame* pInstFrame);

private:
	ERxInstructionFrame	m_InstructionFrame;
	RingBuffer			m_RingBuffer;
};

#endif // SERIALCOMHELPER_H