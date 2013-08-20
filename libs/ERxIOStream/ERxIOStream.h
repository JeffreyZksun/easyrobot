#pragma once
#ifndef ERX_IOSTREAM_H
#define ERX_IOSTREAM_H

#include <Stream.h>

class ERxIOStream : public Stream
{
public:
	//typedef unsigned char size_t; //It is defined in stdio.h.
	typedef uint8_t BufferType;

public:
	ERxIOStream(BufferType* pBuffer, size_t capacity, size_t size);

public:
	// Override functions from Print
	virtual size_t	write(uint8_t);
	using Print::write; // pull in write(str) and write(buf, size) from Print

	// Override functions from Stream
	virtual int		available(void);
	virtual int		peek(void);
	virtual int		read(void);
	virtual void	flush(void);

public:
	// Support decimal and hexadecimal
	unsigned long	parseULong();


public:
	void			resetIterator(size_t readIterator, size_t writeIterator);
	
	BufferType*		rdbuf();

private:
	int				putback();
	unsigned long	parseULong(char skipChar);
	int				readNextDigit();

private:
	bool			full() const ;

private:
	BufferType* 	m_pBuffer;
	size_t 			m_capacity;

	size_t 			m_readIterator;
	size_t 			m_writeIterator;
};

#endif // ERX_IOSTREAM_H