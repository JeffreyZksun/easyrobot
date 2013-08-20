/**************************************************************************************
Functionality:	
<Describe the main functionality of the code in this file.>

The circuit:
<Describe the used circuit connection.>

More:
<Add more information regarding the hardware>

Occupied MCU resource:
<Indicate which resources are occupied by this functionality. 
When try to implement other functionalities, these resources shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
// look for IR codes and print them as they are received
#include <Arduino.h>
#include <NECIRrcv.h>
#define IRPIN 11    // pin that IR detector is connected to

NECIRrcv ir(IRPIN) ;

void setup()
{
	Serial.begin(9600) ;
	Serial.println("NEC IR code reception") ;
	ir.begin() ;
}

void loop()
{
	unsigned long ircode ;

	while (ir.available()) {
		ircode = ir.read() ;
		Serial.print("got code: 0x") ;
		Serial.println(ircode,HEX) ;
	}
}
