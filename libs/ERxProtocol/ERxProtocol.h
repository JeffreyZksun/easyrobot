/***********************************************************************
Declare the classes:
	ERxParameterStream
	ERxInstructionFrame

ERxParameterStream
	A read/write stream to store the command parameter.
	It can encode data of all the built-in type into the byte stream.
	And decode the data with the specified type from the stream.

ERxInstructionFrame
	It maintains the instruction frame protocol. (The map between instruction frame and raw byte stream.)
	This protocol is used for transferring data through small devices byte by byte.

	It maintains the command structure.
	The command is composed by the fixed length command id and various length parameter.

	It can decode the valid instruction from the raw byte stream.
	It can encode a format instruction to the raw byte stream.
	Normally the instruction is transferred through the raw byte stream cross the devices.

************************************************************************/

#pragma once
#ifndef ERXPROTOCOL_H
#define ERXPROTOCOL_H

/***************************************************************************
Refer to the sample Samples\CommunicationProtocol.cpp to get the detail how to create
and response to the instruction command.
***************************************************************************/

#include "ERxMacros.h"
#include "ERxType.h"
#include "ERxRingBuffer.h"

typedef ERxRingBuffer<ERUINT8, RING_BUFFER_SIZE> RingBuffer;

/**********************ERxParameterStream**************************************

***************************************************************************/
#define DECLARE_READ_WRITE_DATA(TYPE) \
	void Write(TYPE data);\
	void Read(TYPE* pData);

#define DECLARE_READ_WRITE_ARRAY(TYPE) \
	void Write(TYPE* pArray, ERINT32 length);\
	void Read(TYPE* pArray, ERINT32 length);

class ERxParameterStream
{
	friend class ERxInstructionFrame;
public:
	ERxParameterStream();

	// The default implementation(bitwise copy) for copy constructor and = operator is enough for us
	//ERxParameterStream(const ERxParameterStream&);
	//ERxParameterStream& operator=(const ERxParameterStream&);

public:

	DECLARE_READ_WRITE_DATA(ERUINT8);
	DECLARE_READ_WRITE_DATA(ERINT8);
	DECLARE_READ_WRITE_DATA(ERINT32);
	DECLARE_READ_WRITE_DATA(ERUINT32);
	DECLARE_READ_WRITE_DATA(ERFLOAT32);

	DECLARE_READ_WRITE_ARRAY(ERINT8);
	DECLARE_READ_WRITE_ARRAY(ERINT32);

	void Write(ERINT8* pStr);
	void Read(ERINT8** ppStr);

	bool Empty() const;

	// Set all the bytes to be 0.
	void Reset();

protected:
	//public: // uncomment it for test.
	void ResetRawBuffer(ERUINT8* pRawParameterBuffer, ERINT32 bufferSize);

public:
	ERINT32				GetBufferSize() const;
	const ERUINT8*		GetBufferAddress() const;

private:
	template<class T>
	void			_Write(T data);
	template<class T>
	void			_Read(T* pData);

private:
	void __Write(ERUINT8* pData, ERINT32 length);
	void __Read(ERUINT8* pData, ERINT32 length);

private:
	//ERUINT8						m_pWriteBuffer[MAX_PARAMETER_LENGTH];
	ERUINT8						m_pWorkingBuffer[MAX_PARAMETER_LENGTH];
	ERINT32						m_iBufferSize;
	ERINT32						m_iReadCursor;
};

/**********************ERxInstructionFrame**************************************
Maintain the data format of the protocol.
***************************************************************************/

class ERxInstructionFrame
{
public:
	ERxInstructionFrame();
	ERxInstructionFrame(const ERxInstructionFrame& src);
	~ERxInstructionFrame();

	void				SetInstruction(ERUINT8 sourceDevice
								, ERUINT8 targetDevice
								, ERUINT8 instruction
								, const ERxParameterStream& parameterStream);

	ERUINT8				GetSourceDevice()  const;
	ERUINT8				GetTargetDevice()  const;
	ERUINT8				GetCommand()  const;
	ERxParameterStream*	GetParameterStream(); 

	ERUINT8				GetFrameDataLength() const;
	const ERUINT8*		GetFrameDataBuffer() const;

	bool				Valid() const;
	void				Invalidate();

	// Analyze the buffer to get the valid instruction stream if there is.
	// If the current object already contains a valid instruction, that instruction will be overridden.
	void				Update(RingBuffer* pBuffer);	

private:
	ERUINT8				GetParameterLength() const;
	const ERUINT8*		GetParameterBuffer() const; 

private:
	ERUINT8						m_pFrameBuffer[MAX_FAME_LENGTH];
	bool						m_bValid;
	ERxParameterStream			m_ParameterStream;
};

#endif