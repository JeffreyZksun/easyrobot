#include <ERxCore.h>

/**************************************************************************************
Functionality:	
Two Arduinos communicate via SPI. One is master, the other is slave. 
They can not send data at once. The Master send data and the Slave read the data.

The circuit:
 Use atmega 328p and atmega 1280 as test boards.
 ATMega328p				ATMega1280
	SS(10)	--------------	SS(53)
	MOSI(11)--------------	MOSI(51)
	MISO(12)--------------	MISO(50)
	SCK(13)	--------------	SCK(52)	

	GND		--------------	GND
	+5V		--------------	+5V

Connect the atmega 328p/1280 board to PC through USB. Monitor the data from it.

More:
<Add more information regarding the hardware>

Occupied MCU resource:
<Indicate which resources are occupied by this functionality. 
When try to implement other functionalities, these resources shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============

#include "pins_arduino.h"

void SPI_Init(void);
unsigned char SPI_ReadWrite(unsigned char data);

#define SPI_MASTER_MODEL
//#define SPI_SLAVE_MODEL

#if defined(SPI_MASTER_MODEL)
	const char* pSPIStr = "Master";
#else
	const char* pSPIStr = "Slavee";// The same length
#endif
	
// init as SPI-Master
void SPI_MASTER_Init(void) {
	pinMode(SCK,OUTPUT);
	pinMode(MOSI,OUTPUT);
	pinMode(SS,OUTPUT);
	pinMode(MISO,INPUT);

	// INIT interface, Master, set clock rate fck/4
	// SPCR ¨C SPI Control Register
	//
	// SPE: SPI Enable. When the SPE bit is written to one, the SPI is enabled. This bit must be set to enable any SPI operations.
	// MSTR: Master/Slave Select. This bit selects Master SPI mode when written to one, and Slave SPI mode when written logic zero.
	// If SS is configured as an input and is driven low while MSTR is set, MSTR will be cleared, and SPIF in SPSR will become set.
	// The user will then have to set MSTR to re-enable SPI Master mode.
	// SPR1, SPR0: SPI Clock Rate Select 1 and 0. These two bits control the SCK rate of the device configured as a Master. 
	// SPR1 and SPR0 have no effect on the Slave.

	SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);
	// enable double rate
	// SPSR ¨C SPI Status Register
	//
	// SPI2X: Double SPI Speed Bit.When this bit is written logic one the SPI speed (SCK Frequency) will be doubled when the SPI
	// is in Master model.
	SPSR = (1<<SPI2X); // we will now gain fck/2 instead of fck/4

	digitalWrite(SCK, LOW);
	digitalWrite(MOSI, LOW);
	digitalWrite(SS, HIGH); 
}

void SPI_SLAVE_Init(void) {
	pinMode(SCK,INPUT);
	pinMode(MOSI,INPUT);
	pinMode(SS,INPUT);
	pinMode(MISO,OUTPUT);

	// INIT interface, Slave, set clock rate fck/4
	SPCR = (1<<SPE)|(0<<MSTR)|(0<<SPR1)|(0<<SPR0);
	// enable double rate
	SPSR = (1<<SPI2X); // we will now gain fck/2 instead of fck/4
}

unsigned char SPI_ReadWrite(unsigned char data) {

#if defined(SPI_MASTER_MODEL)
	digitalWrite(SS, LOW); 


	// Master write, Slave read only
	//
	// set data to send into SPI data register
	//  SPDR ¨C SPI Data Register. The SPI Data Register is a read/write register used for data transfer between the Register File
	// and the SPI Shift Register. Writing to the register initiates data transmission. Reading the register causes 
	// the Shift Register Receive buffer to be read.
	SPDR = data;
#endif
	// Wait for transmission complete
	// SPIF: SPI Interrupt Flag.
	// When a serial transfer is complete, the SPIF Flag is set. An interrupt is generated if SPIE in
	// SPCR is set and global interrupts are enabled. If SS is an input and is driven low when the SPI is
	//	in Master mode, this will also set the SPIF Flag. SPIF is cleared by hardware when executing the
	//	corresponding interrupt handling vector. Alternatively, the SPIF bit is cleared by first reading the
	//	SPI Status Register with SPIF set, then accessing the SPI Data Register (SPDR).

	while(!(SPSR & (1<<SPIF)));

#if defined(SPI_MASTER_MODEL)
	digitalWrite(SS, HIGH); 
#endif

	// return data read from SPI (if any)
	return SPDR;
}

void setup() {

#if defined(SPI_MASTER_MODEL)
	SPI_MASTER_Init();
#else
	SPI_SLAVE_Init();
#endif

	Serial.begin(9600);

}

void loop(){

	Serial.println();
	Serial.println("************SPI communication begin.*************");
	Serial.println();
	const char* str = pSPIStr;
	while (*str)
	{
		unsigned char dataBuffer = (unsigned char)(*str);
		//Serial.print(dataBuffer, BYTE);
		dataBuffer = SPI_ReadWrite(dataBuffer);
		str++;
		Serial.print(dataBuffer, BYTE);
	}
}