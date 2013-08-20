/*
This class can append the prefix to each message frame. The message frames are separated by "\r\n".

This class caches the received data other than send them immediately.
The cached data is sent when any of the following conditions is met:
 1. The cache buffer is full.
 2. The end of the message frame is detected, which is "\r\n".
 3. The flush function is called.

 Use this class to send the data to XBee (AP=0, RO=0x20), it can make sure a message frame is packed into a package when send it to air.

* Created 2 June 2012
* By Jeffrey Sun
* 

*/

#pragma once
#ifndef ERX_REDIRECT_OUTPUT_STREAM_H
#define ERX_REDIRECT_OUTPUT_STREAM_H

#include <ERxOutputStream.h>

class ERxRedirectOutputStream : public ERxOutputStream
{
public:
	ERxRedirectOutputStream(Print* pPrint, BufferType* pBuffer, size_t capacity);

public:
	void			SetPrefix(const BufferType* pPrefix, size_t prefixLength);

public:
	// Override functions from Print
	virtual size_t	write(uint8_t);
	using Print::write; // pull in write(str) and write(buf, size) from Print

public:
	// Override functions from ERxOutputStream
	virtual void	flush(void);

private:
	size_t			_writeToCache(uint8_t);
	bool			full() const;

private:
	typedef struct 
	{
		unsigned char bAddPrefix: 1;
		unsigned char bIsFirstChar: 1;
		unsigned char bPreviousCharIsCR: 1;
	} StatusFlag;


private:
	Print*					m_pActualPrint;

	BufferType* 			m_pBuffer;
	size_t 					m_capacity;
	size_t 					m_writeIterator;

	StatusFlag				m_status;
	const BufferType*		m_pPrefix;
	size_t					m_prefixLength;


};

#endif // ERX_REDIRECT_OUTPUT_STREAM_H