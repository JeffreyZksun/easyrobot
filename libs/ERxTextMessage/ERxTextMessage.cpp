#include "ERxTextMessage.h"
#include <Stream.h>

/************************************************************************/
/*               ERxIORingBufferStream                                  */
/************************************************************************/

#define NO_SKIP_CHAR  1  // a magic char not found in a valid ASCII numeric field

ERxIORingBufferStream::ERxIORingBufferStream()
		: m_iHeadIndex(0)
		, m_iTailIndex(0)
{
	// Initialize the data
	m_Buffer[m_iTailIndex] = '\0';

}

// Append one char to the buffer
size_t ERxIORingBufferStream::write(uint8_t c)
{
	int i = (m_iHeadIndex + 1) % BUFFER_CAPACITY;

	m_Buffer[m_iHeadIndex] = c;
	m_iHeadIndex = i;

	return 1;
}

// Return the length of the available chars.
int ERxIORingBufferStream::available(void)
{
	return (unsigned int)(BUFFER_CAPACITY + m_iHeadIndex- m_iTailIndex) % BUFFER_CAPACITY;
}

// Read a char, but NOT move the tail index.
int ERxIORingBufferStream::peek(void)
{
	if (m_iHeadIndex == m_iTailIndex) {
		return -1;
	} else {
		return m_Buffer[m_iTailIndex];
	}
}

// Read a char and move the tail index.
int ERxIORingBufferStream::read(void)
{
	// if the head isn't ahead of the tail, we don't have any characters
	if (m_iHeadIndex == m_iTailIndex) {
		return -1;
	} else {
		unsigned char c = m_Buffer[m_iTailIndex];
		m_iTailIndex = (unsigned int)(m_iTailIndex + 1) % BUFFER_CAPACITY;
		return c;
	}
}

// Discard all data
void ERxIORingBufferStream::flush(void)
{
	m_iTailIndex = m_iHeadIndex;
}

unsigned int ERxIORingBufferStream::capacity()
{
	return (BUFFER_CAPACITY - 1);
}

bool ERxIORingBufferStream::full()
{
	return (available() == capacity());
}

// Return the distance from the tail to the beginning of the target string.
// -1 means not found.
int ERxIORingBufferStream::peekFind(const char *target, unsigned int length)
{
	return peekFindUntil(target, length, NULL, 0);
}

int ERxIORingBufferStream::peekFindUntil(const char *target, unsigned int targetLen, const char *terminator, unsigned int termLen)
{
	if( *target == 0)
		return 0;   // return true if target is a null string

	// Save
	unsigned int oldTail = m_iTailIndex;
	unsigned int oldHead = m_iHeadIndex;

	int distance = 0;
	size_t index = 0;  // maximum target string length is 64k bytes!
	size_t termIndex = 0;
	int c;

	bool bFound = false;

	while( (c = read()) > 0){

		if(c != target[index])
			index = 0; // reset index if any char does not match

		if( c == target[index]){
			//Serial.print("found "); Serial.print(c); Serial.print("index: "); Serial.print(index+1); Serial.print("dist: "); Serial.println(distance);
			if(++index >= targetLen){ // set true if all chars in the target match
				bFound = true;
				break;
			}
		}

		if(termLen > 0 && c == terminator[termIndex]){
			if(++termIndex >= termLen)
			{
				bFound = false;
				break;       // set false if terminate string found before target string
			}
		}
		else
			termIndex = 0;

		distance++;
	}

	// Recover 
	m_iTailIndex = oldTail;
	m_iHeadIndex = oldHead;

	return bFound ? (distance - targetLen + 1) : -1;
}

unsigned int ERxIORingBufferStream::writeTo(ERxIORingBufferStream* pTargetBuffer, unsigned int length)
{
	if(NULL == pTargetBuffer)
		return 0;

	unsigned int minLength = min(length, this->available() );
	minLength = min(length, pTargetBuffer->capacity() - pTargetBuffer->available());

	for (unsigned int i = 0; i < minLength; i++)
	{
		pTargetBuffer->write((uint8_t)(this->read()));
	}

	return minLength;
}

// Put the last char back to buffer. 
// Return the char if success.
// Return -1 on fail.
int ERxIORingBufferStream::putback()
{
	if(full())
		return -1;

	m_iTailIndex = (m_iTailIndex + BUFFER_CAPACITY - 1) % BUFFER_CAPACITY;

	return peek();
}

unsigned long ERxIORingBufferStream::parseULong()
{
	return parseULong(NO_SKIP_CHAR); // terminate on first non-digit character (or timeout)
}

unsigned long ERxIORingBufferStream::parseULong(char skipChar)
{
	int c;
	c = readNextDigit(); // ignore non numeric leading characters

	if(c < 0)
		return 0; // zero returned if timeout

	boolean isHex = false;
	boolean firstDigitIsZero = false;
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

int ERxIORingBufferStream::readNextDigit()
{
	int c;
	while (1) {
		c = read();
		if (c < 0) return c;
		if (c >= '0' && c <= '9') return c;
	}
}


/************************************************************************/
/*                    ERxTextMessage                                   */
/************************************************************************/



ERxTextMessage::ERxTextMessage(Stream *pIORawStream)
	: m_pIORawStream(pIORawStream)
	, m_RawDataCache()
	, m_ReceivedMessage()
	, m_bReceivedMessageOverflow(false)
	, m_bRawDataBufferOverflow(false)
	, m_bIsDataTransferBegin(false)
{

}

size_t ERxTextMessage::write(uint8_t c)
{
	if(m_pIORawStream != NULL)
		return m_pIORawStream->write(c);

	return 0;
}

int ERxTextMessage::available(void)
{
	// 1. If there already exit a message frame, return directly.
	if(_availableInternal())
		return _availableInternal();

	const char* pMessageTernimator = m_bIsDataTransferBegin ? MULTI_LINES_TREMINATOR : ONE_LINE_TREMINATOR;


	// 2.1 Read data from io stream and cache it.
	if(m_pIORawStream != NULL)
	{
		while(m_pIORawStream->available() && !m_RawDataCache.full())
		{
			m_RawDataCache.write(m_pIORawStream->read());
		}
	}

	unsigned int terminatorLength =strlen(pMessageTernimator);

	// 2.2 Discard the corruption data if the buffer is overflow during the last analysis.
	if(m_bRawDataBufferOverflow)
	{
		// Discard the data before and including the terminator.
		bool bEixt = m_RawDataCache.find(const_cast<char*>(pMessageTernimator), terminatorLength);
		if(bEixt)
		{
			m_bRawDataBufferOverflow = false;
		}
		else
		{
			// The corruption data isn't clear yet. Return directly.
			return _availableInternal();
		}
	}

	// 3. Try to parse a new message frame from the raw data cache.
	bool bHandleKeywords = true;
	int distance = m_RawDataCache.peekFind(pMessageTernimator, terminatorLength);
	if(distance != -1)
	{
		// Found.
		if(m_bIsDataTransferBegin)
			bHandleKeywords = false;
		else
			m_ReceivedMessage.flush(); // Clear the buffer

		m_bReceivedMessageOverflow = false;
		m_bIsDataTransferBegin = false; // Data transfer finished.

		unsigned int totalLen = distance + terminatorLength;
		// Copy the message frame, including the terminator.
		unsigned int writeLen = m_RawDataCache.writeTo(&m_ReceivedMessage, totalLen);
		if(writeLen != totalLen)
		{
			// Overflow: The received data frame is too long.
			m_bReceivedMessageOverflow = true;

			// Discard the left data of the current message frame in the cache.
			unsigned int leftLen = totalLen - writeLen;
			for(unsigned int i = 0; i < leftLen; i++)
			{
				m_RawDataCache.read();
			}

			// The data is truncated. But we still retain it. So don't flush m_CurrentMessageFrame here.
		}
	}
	else
	{
		// Not found
		if(m_RawDataCache.full())
		{
			// Overflow: The received data frame is too long.
			m_bRawDataBufferOverflow = true;

			// We need to discard all the cached data. Otherwise the following received data won't be read.
			m_RawDataCache.flush();

			invalidate();
		}
	}

	// 4. Handle the internal keywords
	// The length of the keyword is 4. And the fifth char is space or \r.

	if(bHandleKeywords && (m_ReceivedMessage.available() >= (KEYWORD_LENGTH + 1)))
	{
		bool bPutback = true;

		char buffer[KEYWORD_LENGTH + 1];
		m_ReceivedMessage.readBytes(buffer, KEYWORD_LENGTH + 1);

		bool bIsSpace = (' ' == buffer[KEYWORD_LENGTH]);
		if(bIsSpace || '\r' == buffer[KEYWORD_LENGTH]) // Check if the fifth char is space or \r.
		{
			// Change all chars to upper case
			for (int i = 0; i < KEYWORD_LENGTH; i++) 
				buffer[i] = toupper(buffer[i]);

			buffer[KEYWORD_LENGTH] = '\0'; // for strcmp
			if(strcmp(buffer, CONTROL_STRING_DATA) == 0)
			{
				// Data
				m_bIsDataTransferBegin = true;
			}
			else if(strcmp(buffer, CONTROL_STRING_ECHO) == 0)
			{
				// Echo

				//print(CONTROL_STRING_ECHO); print(bIsSpace ? ' ' : '\r'); // Don't send back the "echo" to avoid the deak lock.
				m_ReceivedMessage.putback();
				while(m_ReceivedMessage.available())
					write((uint8_t)(m_ReceivedMessage.read()));

				m_ReceivedMessage.flush();

				bPutback = false;
			}
		}

		if(bPutback)
		{
			for(int i = 0; i < (KEYWORD_LENGTH + 1); i++)
				m_ReceivedMessage.putback();
		}
	}

	//Serial.print("m_bReceivedMessageOverflow: ");Serial.println(m_bReceivedMessageOverflow);
	//Serial.print("m_bRawDataBufferOverflow: ");Serial.println(m_bRawDataBufferOverflow);
	//Serial.print("m_bIsDataTransferBegin: ");Serial.println(m_bIsDataTransferBegin);

	// 5. Return the new analysis result
	return _availableInternal();
}

int ERxTextMessage::peek(void)
{
	return m_ReceivedMessage.peek();
}

int ERxTextMessage::read(void)
{
	return m_ReceivedMessage.read();
}

void ERxTextMessage::flush(void)
{
	if(m_pIORawStream != NULL)
		m_pIORawStream->flush();
}

// Discard the current frame
void ERxTextMessage::invalidate()
{
	m_bIsDataTransferBegin = false;
	m_bReceivedMessageOverflow = false;
	m_ReceivedMessage.flush();
}

unsigned long ERxTextMessage::parseULong()
{
	return m_ReceivedMessage.parseULong();
}

int  ERxTextMessage::_availableInternal(void)
{
	if(m_ReceivedMessage.available() && !m_bIsDataTransferBegin)
		return m_ReceivedMessage.available();

	return 0;
}