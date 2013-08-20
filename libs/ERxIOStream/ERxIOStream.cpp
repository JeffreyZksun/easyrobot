#include "ERxIOStream.h"

#define NO_SKIP_CHAR  1  // a magic char not found in a valid ASCII numeric field


ERxIOStream::ERxIOStream(BufferType* pBuffer, size_t capacity, size_t size)
: m_pBuffer(pBuffer), m_capacity(capacity), m_readIterator(0), m_writeIterator(size)
{
}

size_t ERxIOStream::write(uint8_t c)
{
	if(!m_pBuffer)
		return 0;

	if(full())
		return 0;

	m_pBuffer[m_writeIterator] = c;
	++m_writeIterator;

	return 1;
}


int ERxIOStream::available(void)
{
	return (m_writeIterator - m_readIterator);
}

// Read a char, but NOT move the tail index.
int ERxIOStream::peek(void)
{
	if(!available())
		return -1;

	return m_pBuffer[m_readIterator];
}

int ERxIOStream::read(void)
{
	if(!available())
		return -1;

	return m_pBuffer[m_readIterator++];
}

void ERxIOStream::flush(void)
{
	m_readIterator = 0;
	m_writeIterator = 0;
}

void ERxIOStream::resetIterator(size_t readIterator, size_t writeIterator)
{
	m_readIterator = readIterator;
	m_writeIterator = writeIterator;
}

ERxIOStream::BufferType* ERxIOStream::rdbuf()
{
	return m_pBuffer;
}

bool ERxIOStream::full() const 
{
	return m_writeIterator == m_capacity;
}

unsigned long ERxIOStream::parseULong()
{
	return parseULong(NO_SKIP_CHAR); // terminate on first non-digit character (or timeout)
}

// Put the last char back to buffer. 
// Return the char if success.
// Return -1 on fail.
int ERxIOStream::putback()
{
	if(0 == m_readIterator)
		return -1;

	m_readIterator--;

	return peek();
}

unsigned long ERxIOStream::parseULong(char skipChar)
{
	int c;
	c = readNextDigit(); // ignore non numeric leading characters

	if(c < 0)
		return 0; // zero returned if timeout

	bool isHex = false;
	bool firstDigitIsZero = false;
	unsigned long value = 0;

	unsigned int digitIndex = 1;

	putback();

	// 0xF1A2
	// 1584
	do{
		if(c == skipChar)
			; // ignore this character
		else
		{
			if(1 == digitIndex && '0' == c)
			{
				firstDigitIsZero = true;
			}
			else if(2 == digitIndex && true == firstDigitIsZero && ('x' == c || 'X' == c))
			{
				isHex = true;
			}
			else
			{
				if(isHex)
				{
					if(c >= '0' && c <= '9')
					{
						value = value * 16 + c - '0';
					}
					else if(c >= 'a' && c <= 'f')
					{
						value = value * 16 + c - 'a' + 10;
					}
					else if(c >= 'A' && c <= 'F')
					{
						value = value * 16 + c - 'A' + 10;
					}
					else
					{
						break; // Invalid char
					}
					//Serial.print("c=");Serial.write(c);Serial.print("value=");Serial.print(value);
				}
				else
				{
					if(c >= '0' && c <= '9')
					{
						value = value * 10 + c - '0';
					}
					else
					{
						break; // Invalid char
					}
				}
			}
		}

		read();

		c =  peek();
		digitIndex++;
	}
	while(true);

	return value;
}

int ERxIOStream::readNextDigit()
{
	int c;
	while (1) {
		c = read();
		if (c < 0) return c;
		if (c >= '0' && c <= '9') return c;
	}
}
