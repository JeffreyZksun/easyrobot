#ifndef ERxTextMessage_H
#define ERxTextMessage_H
#include <Arduino.h>

class Stream;

/************************************************************************/
/*               Define the constants used in the protocol              */
/************************************************************************/
#define ONE_LINE_TREMINATOR "\r\n"
#define MULTI_LINES_TREMINATOR "\r\n.\r\n"

// The length of the key words are 4. The keywords defined here should be upper cases.
#define KEYWORD_LENGTH 4
#define CONTROL_STRING_DATA "DATA"
#define CONTROL_STRING_ECHO "ECHO"

#define NUM_OK 50


/************************************************************************/
/*               ERxIORingBufferStream                                  */
/************************************************************************/

#define BUFFER_CAPACITY 128

// The max capacity is BUFFER_CAPACITY-1
class ERxIORingBufferStream : public Stream
{
public:
	ERxIORingBufferStream();

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
	bool			full();
	unsigned int	capacity();
	// Return the distance from the tail to the beginning of the target string.
	// -1 means not found.
	int				peekFind(const char *target, unsigned int length); 
	unsigned int	writeTo(ERxIORingBufferStream* pTargetBuffer, unsigned int length);

	int				putback();

public:
	unsigned long	parseULong();

private:
	unsigned long	parseULong(char skipChar);
	int				readNextDigit();

private:
	int				peekFindUntil(const char *target, unsigned int targetLen
									, const char *terminator, unsigned int termLen);

private:
	char			m_Buffer[BUFFER_CAPACITY];
	unsigned int	m_iHeadIndex; 
	unsigned int	m_iTailIndex;
};

/************************************************************************/
/*                    ERxTextMessage                                   */
/************************************************************************/

class ERxTextMessage : public Stream
{
public:
	ERxTextMessage(Stream *pIORawStream);

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
	void			invalidate();

public:
	unsigned long	parseULong();

private:
	int				_availableInternal(void);

private:

	Stream*					m_pIORawStream;
	ERxIORingBufferStream	m_RawDataCache;
	ERxIORingBufferStream	m_ReceivedMessage;

	bool					m_bReceivedMessageOverflow;
	bool					m_bRawDataBufferOverflow;
	bool					m_bIsDataTransferBegin; // If true, the DATA is received and the following chars are data body

};

#endif // ERxTextMessage_H