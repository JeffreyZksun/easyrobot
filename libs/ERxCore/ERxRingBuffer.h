/***********************************************************************
Declare the classes:
	ERxRingBuffer

ERxRingBuffer
	A template class which uses the ring buffer to save the data.
	It can be used to save the POD data.
	This class can be used as a first-in-first-out buffer. 

************************************************************************/

#pragma once
#ifndef ERXRINGBUFFER_H
#define ERXRINGBUFFER_H

#include "ERxType.h"

template<class _Ty = char, ERUINT32 BUFFER_CAPACITY = 128>
class ERxRingBuffer
{
public:
	typedef _Ty value_type;
	typedef ERUINT32 size_type;

	ERxRingBuffer(): m_iHeadIndex(0), m_iTailIndex(0), m_BufferCapacity(BUFFER_CAPACITY)
	{
		if(m_BufferCapacity < 1)
			m_BufferCapacity = 1;

		// Initialize the data.
		for(size_type i = 0; i < m_BufferCapacity; i++)
			m_Buffer[i] = value_type();

	}

	ERxRingBuffer(const ERxRingBuffer& src): m_iHeadIndex(0), m_iTailIndex(0), m_BufferCapacity(BUFFER_CAPACITY)
	{
		if(m_BufferCapacity < 1)
			m_BufferCapacity = 1;

		// Don't copy data if the source is not equal to the current one.
		if(src.m_BufferCapacity != m_BufferCapacity)
		{
			for(size_type i = 0; i < m_BufferCapacity; i++)
				m_Buffer[i] = value_type();
			return; 
		}

		m_iHeadIndex = src.m_iHeadIndex;
		m_iTailIndex = src.m_iTailIndex;
		for (size_type i = 0; i < m_BufferCapacity; i++)
		{
			m_Buffer[i] = src.m_Buffer[i];
		}
	}

	void Append(value_type data)
	{
		size_type i = (m_iHeadIndex + 1) % m_BufferCapacity;

		// if we should be storing the received character into the location
		// just before the tail (meaning that the head would advance to the
		// current location of the tail), we're about to overflow the buffer
		// and so we don't write the character or advance the head.
		if (i != m_iTailIndex) {
			m_Buffer[m_iHeadIndex] = data;
			m_iHeadIndex = i;
		}
	}

	value_type	Pop()
	{
		if (m_iHeadIndex == m_iTailIndex) {
			return value_type();
		} else {
			value_type c = m_Buffer[m_iTailIndex];
			m_iTailIndex = (m_iTailIndex + 1) % m_BufferCapacity;
			return c;
		}
	}

	void Flush()
	{
		m_iTailIndex = m_iHeadIndex;
	}

	bool Empty() const
	{
		return Size() == 0;
	}

	size_type Size() const
	{
		return (m_BufferCapacity + m_iHeadIndex - m_iTailIndex) % m_BufferCapacity;
	}

	bool Full() const
	{
		return Size() == (m_BufferCapacity - 1);
	}

	size_type	GetTailIndex() const
	{
		return m_iTailIndex;
	}

	void ResetTailIndex(size_type tailIndex)
	{
		m_iTailIndex = tailIndex % m_BufferCapacity;;
	}

	size_type Capacity() const
	{
		return m_BufferCapacity;
	}

private:
	// Take care: the maximum data we can buffer is BUFFER_CAPACITY - 1.Not BUFFER_CAPACITY.
	value_type		m_Buffer[BUFFER_CAPACITY];
	size_type		m_iHeadIndex; 
	size_type		m_iTailIndex;
	size_type		m_BufferCapacity;
};

#endif