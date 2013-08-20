#include "ERxBufferOutputStream.h"

ERxBufferOutputStream::ERxBufferOutputStream(BufferType* pBuffer, size_t capacity)
: m_pBuffer(pBuffer), m_capacity(capacity), m_writeIterator(0)
{
}

size_t ERxBufferOutputStream::write(uint8_t c)
{
	if(!m_pBuffer)
		return 0;

	if(full())
		return 0;

	m_pBuffer[m_writeIterator] = c;
	++m_writeIterator;
	return 1;

}

ERxBufferOutputStream::BufferType* ERxBufferOutputStream::rdbuf()
{
	return m_pBuffer;
}

size_t ERxBufferOutputStream::size()
{
	return m_writeIterator;
}

void ERxBufferOutputStream::flush(void)
{
	m_writeIterator = 0;
}

bool ERxBufferOutputStream::full() const 
{
	return m_writeIterator == m_capacity;
}
