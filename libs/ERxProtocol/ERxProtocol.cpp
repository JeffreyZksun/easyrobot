
#include "ERxProtocol.h"

/***************************ERxParameterStream**********************************/

#define IMPLEMENT_READ_WRITE_DATA(CLS, TYPE) \
	void CLS::Write(TYPE data)\
{\
	_Write(data);\
}\
	void CLS::Read(TYPE* pData)\
{\
	_Read(pData);\
}

#define IMPLEMENT_READ_WRITE_ARRAY(CLS, TYPE) \
	void CLS::Write(TYPE* pArray, ERINT32 length)\
{\
	for (int i = 0; i < length; i++)\
	Write(pArray[i]);\
}\
	void CLS::Read(TYPE* pArray, ERINT32 length)\
{\
	for (int i = 0; i < length; i++)\
	Read(&pArray[i]);\
}

ERxParameterStream::ERxParameterStream()
: /*m_pWorkingBuffer,*/ m_iBufferSize(0), m_iReadCursor(0)
{
	Reset();
}

IMPLEMENT_READ_WRITE_DATA(ERxParameterStream, ERUINT8);
IMPLEMENT_READ_WRITE_DATA(ERxParameterStream, ERINT8);
IMPLEMENT_READ_WRITE_DATA(ERxParameterStream, ERINT32);
IMPLEMENT_READ_WRITE_DATA(ERxParameterStream, ERUINT32);
IMPLEMENT_READ_WRITE_DATA(ERxParameterStream, ERFLOAT32);

IMPLEMENT_READ_WRITE_ARRAY(ERxParameterStream, ERINT8);
IMPLEMENT_READ_WRITE_ARRAY(ERxParameterStream, ERINT32);

// End with '\0'
void ERxParameterStream::Write(ERINT8* pStr)
{
	ERINT32 length = 0;
	while(pStr[length] != '\0' && length < (MAX_PARAMETER_LENGTH - 1))
		length++;

	length++; // write '\0'

	Write(pStr, length);
}

// End with '\0'
void ERxParameterStream::Read(ERINT8** ppStr)
{
	ERINT32 length = 0;

	for(; !Empty(); length++)
	{
		ERINT8 tmpChar;
		Read((ERUINT8*)&tmpChar);
		(*ppStr)[length] = tmpChar;
		if(tmpChar == '\0')
			return;
	}

	(*ppStr)[length] = '\0'; // Append '\0' if the buffer is empty.
}

bool ERxParameterStream::Empty() const
{
	return m_iReadCursor == m_iBufferSize;
}

void ERxParameterStream::Reset()
{
	for (int i = 0; i < MAX_PARAMETER_LENGTH; i++)
	{
		m_pWorkingBuffer[i] = 0;
	}

	m_iBufferSize = 0;
	m_iReadCursor = 0;
}

template<class T>
void ERxParameterStream::_Write(T data)
{
	__Write((ERUINT8 *)&data, sizeof(T));
}

template<class T>
void ERxParameterStream::_Read(T* pData)
{
	__Read((ERUINT8 *)pData, sizeof(T));
}

void ERxParameterStream::__Write(ERUINT8* pData, ERINT32 length)
{
	// avoid overflow
	for(ERINT32 i = 0; i < length && m_iBufferSize < MAX_PARAMETER_LENGTH; i++)
	{
		m_pWorkingBuffer[m_iBufferSize] = pData[i];
		m_iBufferSize++;
	}
}

void ERxParameterStream::__Read(ERUINT8* pData, ERINT32 length)
{
	// avoid overflow
	for(ERINT32 i = 0; i < length && m_iReadCursor < m_iBufferSize; i++)
	{
		pData[i] = m_pWorkingBuffer[m_iReadCursor];
		m_iReadCursor++;
	}
}

void ERxParameterStream::ResetRawBuffer(ERUINT8* pRawParameterBuffer, ERINT32 bufferSize)
{
	//m_pWorkingBuffer = pRawParameterBuffer;

	// Copy the data to buffer.
	m_iBufferSize = bufferSize < MAX_PARAMETER_LENGTH ? bufferSize : MAX_PARAMETER_LENGTH; 
	for(int i = 0; i < m_iBufferSize; i++)
		m_pWorkingBuffer[i] = pRawParameterBuffer[i];
	//m_iBufferSize = bufferSize;
	m_iReadCursor = 0;
}

ERINT32	ERxParameterStream::GetBufferSize() const
{
	return m_iBufferSize;
}

const ERUINT8* ERxParameterStream::GetBufferAddress() const
{
	return m_pWorkingBuffer;
}

///**********************ERxInstructionFrame******************************///

ERxInstructionFrame::ERxInstructionFrame():m_bValid(false)
{
}

ERxInstructionFrame::ERxInstructionFrame(const ERxInstructionFrame& src)
{
	SetInstruction(src.GetSourceDevice(), src.GetTargetDevice(), src.GetCommand(), *(((ERxInstructionFrame *)&src)->GetParameterStream()));

	m_bValid = src.m_bValid;
}

void ERxInstructionFrame::SetInstruction(ERUINT8 sourceDevice
										   , ERUINT8 targetDevice
										   , ERUINT8 instruction
										   , const ERxParameterStream& parameterStream)
{
	m_pFrameBuffer[HEADER_0_INDEX] = HEADER_0;
	m_pFrameBuffer[HEADER_1_INDEX] = HEADER_1;
	m_pFrameBuffer[SOURCE_DEVICE_INDEX] = sourceDevice;
	m_pFrameBuffer[TARGET_DEVICE_INDEX] = targetDevice;
	m_pFrameBuffer[INSTRUCTION_CMD_INDEX] = instruction;

	ERINT32 parameterLength = parameterStream.GetBufferSize();

	if(parameterLength > MAX_PARAMETER_LENGTH)
		parameterLength = MAX_PARAMETER_LENGTH;

	m_pFrameBuffer[PARA_LENGTH_INDEX] = (ERUINT8)parameterLength;

	const ERUINT8 * pParameters = parameterStream.GetBufferAddress();
	for(ERINT32 i = 0; i < parameterLength; i++)
		m_pFrameBuffer[PARA_HEADER_INDEX + i] = pParameters[i];

	ERINT32 CrcIndex = PARA_HEADER_INDEX + parameterLength;
	m_pFrameBuffer[CrcIndex + 0] = EMPTY_CRC;
	m_pFrameBuffer[CrcIndex + 1] = EMPTY_CRC;
	m_pFrameBuffer[CrcIndex + 2] = FOOTER_0;
	m_pFrameBuffer[CrcIndex + 3] = FOOTER_1;

	m_ParameterStream.ResetRawBuffer((ERUINT8*)GetParameterBuffer(), GetParameterLength());

	m_bValid = true;
}

ERxInstructionFrame::~ERxInstructionFrame()
{
	// Nothing to do.
}

ERUINT8 ERxInstructionFrame::GetSourceDevice() const
{
	return m_pFrameBuffer[SOURCE_DEVICE_INDEX];
}

ERUINT8 ERxInstructionFrame::GetTargetDevice() const
{
	return m_pFrameBuffer[TARGET_DEVICE_INDEX];
}

ERUINT8 ERxInstructionFrame::GetCommand()  const
{
	return m_pFrameBuffer[INSTRUCTION_CMD_INDEX];
}

ERUINT8 ERxInstructionFrame::GetParameterLength() const
{
	return m_pFrameBuffer[PARA_LENGTH_INDEX];

}

const ERUINT8* ERxInstructionFrame::GetParameterBuffer() const
{
	return &m_pFrameBuffer[PARA_HEADER_INDEX];
}

ERxParameterStream* ERxInstructionFrame::GetParameterStream()
{
	return &m_ParameterStream;
}

ERUINT8 ERxInstructionFrame::GetFrameDataLength() const
{
	return GetParameterLength() + FRAEM_CONTROL_CHARS_LENGTH;
}

const ERUINT8* ERxInstructionFrame::GetFrameDataBuffer() const
{
	return m_pFrameBuffer;
}

bool ERxInstructionFrame::Valid() const
{
	return m_bValid;
}

void ERxInstructionFrame::Invalidate()
{
	m_bValid = false;
}

// Update the ERxInstructionFrame by using the data in buffer.
// The buffer will be also changed during the update.

// If the buffer contains a valid instruction frame, the first frame will be returned, and 
// the buffer tail index will be moved to the end of the instruction frame.
// If the buffer contains a valid frame header, but instruction data isn't received completely,
// the buffer tail index will be moved to the front of the frame header.
// If there aren't whole instruction frame or receiving instruction frame, clear the buffer.
// That means all the invalid buffer data will be discarded.
void ERxInstructionFrame::Update(RingBuffer* pBuffer)
{
	if(!pBuffer || pBuffer->Empty())
		return;

	RingBuffer pCloneBuffer(*pBuffer);
	ERUINT32 iNewTailIndex = pBuffer->GetTailIndex();

	while(!pCloneBuffer.Empty())
	{
		// Find header
		bool findHeader = false;
		while(!findHeader)
		{
			while(pCloneBuffer.Pop() != HEADER_0 && ! pCloneBuffer.Empty()); // Find heading 0
			if(pCloneBuffer.Empty())
				break;

			if(pCloneBuffer.Pop() == HEADER_1)
				findHeader = true;
		}

		if(!findHeader)
		{
			iNewTailIndex = pCloneBuffer.GetTailIndex(); // No valid header.
			break;
		}

		ERUINT32 iTailIndexAfterHeader = pCloneBuffer.GetTailIndex();
		iNewTailIndex = iTailIndexAfterHeader - 2; // The index of the frame header.

		if(pCloneBuffer.Size() < 8) 
		{// Instruction is receiving, we need at least all the data except parameters.
			break;
		}

		// Get the source device  #3
		ERUINT8 sourceDevice = pCloneBuffer.Pop();

		// Get the target device #4
		ERUINT8 targetDevice = pCloneBuffer.Pop();

		// Get the instruction #5
		ERUINT8 instCmd = pCloneBuffer.Pop();

		// Get parameter length #6
		ERUINT32 paramLength = pCloneBuffer.Pop(); 
		if(paramLength > MAX_PARAMETER_LENGTH) // Invalid instruction header
			continue;

		ERUINT32 expectedDataLength = paramLength + 4; //  RCR (2) + ending (2)
		if(pCloneBuffer.Size() < expectedDataLength) // The instruction isn't received completely.
		{
			break; // Instruction is receiving
		}

		ERxParameterStream tmpParameterStream;
		for(ERUINT32 i = 0; i < paramLength; i++)
			tmpParameterStream.Write(pCloneBuffer.Pop());
			//gParameterBuffer[i] = pCloneBuffer.Pop();

		// Check CRC (2 bytes)
		bool bCrcCorrect = true;
		if(pCloneBuffer.Pop() != EMPTY_CRC)
			bCrcCorrect = false;
		if(pCloneBuffer.Pop() != EMPTY_CRC)
			bCrcCorrect = false;

		// Check the ending (2 bytes)
		bool bEndingCorrect = true;
		if(pCloneBuffer.Pop() != FOOTER_0)
			bEndingCorrect = false;
		if(pCloneBuffer.Pop() != FOOTER_1)
			bEndingCorrect = false;

		if(bCrcCorrect && bEndingCorrect)
		{
			iNewTailIndex = pCloneBuffer.GetTailIndex(); // The end of the frame.
			SetInstruction(sourceDevice, targetDevice, instCmd, tmpParameterStream/*gParameterBuffer, paramLength*/);
			break;
		}
		else
		{
			pCloneBuffer.ResetTailIndex(iTailIndexAfterHeader); // Parse the buffer following the invalid header.
			continue;
		}
	}

	// Update the tail index.
	pBuffer->ResetTailIndex(iNewTailIndex);

	return;
}
