#include "SerialComHelper.h"
#include <HardwareSerial.h>

/********************SerialComHelper****************************/

void SerialComHelper::SendInstruction(HardwareSerial* pSerial, ERUINT8 sourceDevice, ERUINT8  targetDevice
					 ,ERUINT8 cmd, const ERxParameterStream& parameterStream)
{
	ERxInstructionFrame tmpInstFrame;
	tmpInstFrame.SetInstruction(sourceDevice, targetDevice, cmd, parameterStream);	// 1. Generate instruction frame.

	Serial.write(tmpInstFrame.GetFrameDataBuffer(), tmpInstFrame.GetFrameDataLength()); // 2. Send the instruction frame.
}

void SerialComHelper::SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
									  , const ERxParameterStream& parameterStream)
{
	SendInstruction(pSerial, DEVICE_BROADCAST, DEVICE_BROADCAST, cmd, parameterStream);
}

ERxInstructionFrame* SerialComHelper::GetFirstInstruction(HardwareSerial* pSerial)
{
	if(NULL == pSerial)
		return NULL;

	while(Serial.available() && !m_RingBuffer.Full())
		m_RingBuffer.Append((ERUINT8)Serial.read()); // 1. Read the data from serial buffer and append it to data ring buffer.

	if(m_RingBuffer.Size() < PARA_HEADER_INDEX) // Not enough data
		return NULL;

	m_InstructionFrame.Invalidate();						
	m_InstructionFrame.Update(&m_RingBuffer);			// 2. Parse the received data and populate the instruction command
	if(m_InstructionFrame.Valid())							// 3. Execute the command if a valid instruction command returned.
		return &m_InstructionFrame;
	else
		return NULL;
}

void SerialComHelper::SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
							,const ERINT8 str[])
{
	ERxParameterStream tempParaStream;
	tempParaStream.Write((ERINT8*)str);

	SendInstruction(pSerial, cmd, tempParaStream);
}

void SerialComHelper::SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
					 ,const char str[])
{
	ERxParameterStream tempParaStream;
	tempParaStream.Write((ERINT8*)str);

	SendInstruction(pSerial, cmd, tempParaStream);
}

void SerialComHelper::SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
							,ERINT8 c)
{
	ERxParameterStream tempParaStream;
	tempParaStream.Write(c);
	SendInstruction(pSerial, cmd, tempParaStream);
}

void SerialComHelper::SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
							,ERUINT8 c)
{
	ERxParameterStream tempParaStream;
	tempParaStream.Write(c);
	SendInstruction(pSerial, cmd, tempParaStream);
}

void SerialComHelper::SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
							,ERINT32 c)
{
	ERxParameterStream tempParaStream;
	tempParaStream.Write(c);
	SendInstruction(pSerial, cmd, tempParaStream);
}

void SerialComHelper::SendInstruction(HardwareSerial* pSerial, ERUINT8 cmd
									  ,ERFLOAT32 c)
{
	ERxParameterStream tempParaStream;
	tempParaStream.Write(c);
	SendInstruction(pSerial, cmd, tempParaStream);
}

ERUINT8 SerialComHelper::ReadParameterAsUChar(ERxInstructionFrame* pInstFrame)
{
	if(NULL == pInstFrame)
		return 0;

	ERUINT8 para = 0;
	pInstFrame->GetParameterStream()->Read(&para);
	return para;
}

ERINT8 SerialComHelper::ReadParameterAsChar(ERxInstructionFrame* pInstFrame)
{
	if(NULL == pInstFrame)
		return 0;

	ERINT8 para = 0;
	pInstFrame->GetParameterStream()->Read(&para);
	return para;
}

ERINT32 SerialComHelper::ReadParameterAsLong(ERxInstructionFrame* pInstFrame)
{
	if(NULL == pInstFrame)
		return 0;

	ERINT32 para = 0;
	pInstFrame->GetParameterStream()->Read(&para);
	return para;
}

ERFLOAT32 SerialComHelper::ReadParameterAsFloat(ERxInstructionFrame* pInstFrame)
{
	if(NULL == pInstFrame)
		return 0.0;

	ERFLOAT32 para = 0.0;
	pInstFrame->GetParameterStream()->Read(&para);
	return para;
}