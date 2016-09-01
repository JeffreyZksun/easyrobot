#pragma once
#ifndef ERX_OUTPUT_STREAM_H
#define ERX_OUTPUT_STREAM_H

#include <Print.h>

class ERxOutputStream : public Print
{
public:
	//typedef unsigned char size_t; //It is defined in stdio.h.
	typedef uint8_t BufferType;

public:

	// using Print::write; // pull in write(str) and write(buf, size) from Print

public:
	size_t		writeP(const char *data, size_t length);
	size_t		printP(const char *str);

public:
	virtual void		flush(void);
	virtual BufferType*	rdbuf();
	virtual size_t		size();
};

#endif // ERX_OUTPUT_STREAM_H