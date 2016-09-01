#include "ERxOutputStream.h"

ERxOutputStream::BufferType* ERxOutputStream::rdbuf()
{
	return 0;
}

size_t ERxOutputStream::size()
{
	return 0;
}

void ERxOutputStream::flush(void)
{

}

size_t ERxOutputStream::writeP(const char *data, size_t length)
{
	size_t n = 0;

	// copy data out of program memory into local storage, write out in
	// chunks of 16 bytes 
	size_t chunk_size = 16;
	uint8_t buffer[chunk_size];
	size_t bufferEnd = 0;

	while (length--)
	{
		if (bufferEnd == chunk_size)
		{
			n += write(buffer, chunk_size);
			bufferEnd = 0;
		}

		buffer[bufferEnd++] = pgm_read_byte(data++);
	}

	if (bufferEnd > 0)
		n += write(buffer, bufferEnd);

	return n;
}

size_t ERxOutputStream::printP(const char *str)
{
	size_t n = 0;

	// copy data out of program memory into local storage, write out in
	// chunks of 16 bytes 
	size_t chunk_size = 16;
	uint8_t buffer[chunk_size];
	size_t bufferEnd = 0;

	while (buffer[bufferEnd++] = pgm_read_byte(str++))
	{
		if (bufferEnd == chunk_size)
		{
			n += write(buffer, chunk_size);
			bufferEnd = 0;
		}
	}

	// write out everything left but trailing NUL
	if (bufferEnd > 1)
		n += write(buffer, bufferEnd - 1);

	return n;
}