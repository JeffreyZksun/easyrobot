#include "ERxRedirectOutputStream.h"

#include <Arduino.h>

// Uncomment this macro if want to print debug message.
//#define DEBUG_DOP

#ifdef DEBUG_DOP
#include <HardwareSerial.h>
#endif


// 100 milliseconds.
unsigned long previous_flush = 0;
#define FLUSH_INTERVAL 100

ERxRedirectOutputStream::ERxRedirectOutputStream(Print* pPrint, BufferType* pBuffer, size_t capacity)
: m_pActualPrint(pPrint), m_pBuffer(pBuffer), m_capacity(capacity), m_writeIterator(0)
, m_status(), m_pPrefix(0), m_prefixLength(0)
{
	m_status.bAddPrefix = true;
	m_status.bIsFirstChar = true;
	m_status.bPreviousCharIsCR = false;
}

size_t ERxRedirectOutputStream::write(uint8_t c)
{
	size_t write_size = 0;
	if(m_pActualPrint != 0)
	{
		if(full())
		{
			flush();
		}

		bool bIsFirstChar = m_status.bIsFirstChar;
		m_status.bIsFirstChar = false; // change it to avoid the recursively call.

		if(bIsFirstChar && m_status.bAddPrefix && m_pPrefix != NULL && m_prefixLength > 0)
		{
			//m_pActualPrint->write(m_pPrefix, m_prefixLength);
			for(size_t i = 0; i < m_prefixLength; ++i)
				_writeToCache(m_pPrefix[i]);
		}

		//write_size = m_pActualPrint->write(c);
		write_size = _writeToCache(c);

		if('\n' == c)
		{
			if(m_status.bPreviousCharIsCR)
			{
				// The end of the message frame is coming. Send out the cached data.
				flush();
				m_status.bIsFirstChar = true;
			}

			m_status.bPreviousCharIsCR = false;
		}
		else if('\r' == c)
			m_status.bPreviousCharIsCR = true;
		else
			m_status.bPreviousCharIsCR = false;
	}

	return write_size;

}

void ERxRedirectOutputStream::SetPrefix(const BufferType* pPrefix, size_t prefixLength)
{
	m_pPrefix = pPrefix;
	m_prefixLength = prefixLength;
}

void ERxRedirectOutputStream::flush(void)
{
	if(m_pActualPrint != NULL && m_pBuffer != NULL && m_writeIterator > 0)
	{
		// Stop an interval between message packages.
		if (millis() < previous_flush)  // Timer overflow
			previous_flush = millis();

		long diff = millis() - previous_flush;
		if (diff < FLUSH_INTERVAL)
		{
			delay(FLUSH_INTERVAL - diff);
#ifdef DEBUG_DOP
			//Serial.println(diff);
#endif
		}

#ifdef DEBUG_DOP
		Serial.print("flush\r\n");
#endif

		m_pActualPrint->write(m_pBuffer, m_writeIterator);
		m_writeIterator = 0;
		m_status.bIsFirstChar = true;
		m_status.bPreviousCharIsCR = false;

		previous_flush = millis();

	}
}

size_t ERxRedirectOutputStream::_writeToCache(uint8_t c)
{
	if(!m_pBuffer || !m_pActualPrint)
		return 0;

	if(full())
	{
		flush();
	}

	m_pBuffer[m_writeIterator] = c;
	++m_writeIterator;

	return 1;
}

bool ERxRedirectOutputStream::full() const 
{
	return m_writeIterator == m_capacity;
}
