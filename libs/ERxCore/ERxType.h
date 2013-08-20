#pragma once
#ifndef ERXTYPE_H
#define ERXTYPE_H

// Empty pointer
#ifndef NULL
#define  NULL 0
#endif

/*************************************************************************
The following table shows the data size on different processors.
We can see that the int type is always 4 chars long in both x32 and x64 processor.

AVR(ATMEGA1280)
type	length
char 	  1
int 	  2
long	  4
float	  4
double	  4

PC 
type	length(x32)  length(x64)
char 	  1				1
short	  2				2
int		  4				4
long	  4				8 (changed)
float	  4				4
double 	  8				8 (changed)
int*	  4				8 (changed)

It is recommended to use the following pairs to transfer data between MCU and PC.
char (AVR)			----- char (PC x32 x64);	size = 1;
unsigned char (AVR) ----- unsigned char (PC x32 x64);	size = 1;
long (AVR)			----- int (PC x32 x64);	size = 4;
float (AVR)			----- float(PC x32 x64);	size = 4;

**************************************************************************/
typedef signed char         ERINT8;
typedef unsigned char       ERUINT8;

#if defined(PC_PROCESSOR) // PC
	typedef signed int		ERINT32;
	typedef unsigned int	ERUINT32;
//#elif defined(__AVR_ATmega1280__) // MCU
#else
	typedef signed long		ERINT32;
	typedef unsigned long   ERUINT32;
#endif

typedef float		        ERFLOAT32;
typedef bool		        ERBOOL;

#endif
