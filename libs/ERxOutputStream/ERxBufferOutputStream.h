#pragma once
#ifndef ERX_BUFFER_OUTPUT_STREAM_H
#define ERX_BUFFER_OUTPUT_STREAM_H

#include <ERxOutputStream.h>

class ERxBufferOutputStream : public ERxOutputStream
{
public:
	ERxBufferOutputStream(BufferType* pBuffer, size_t capacity);

public:
	// Override functions from Print
	virtual size_t	write(uint8_t);
	using Print::write; // pull in write(str) and write(buf, size) from Print

public:
	// Override functions from ERxOutputStream
	virtual void		flush(void);
	virtual BufferType*	rdbuf();
	virtual size_t		size();

private:
	bool				full() const;

private:

	BufferType* 	m_pBuffer;
	size_t 			m_capacity;

	size_t 			m_writeIterator;
};

#endif // ERX_BUFFER_OUTPUT_STREAM_H