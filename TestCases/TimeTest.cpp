#include <ERxCore.h>

/**************************************************************************************
Functionality:	
 Test the time library from http://www.arduino.cc/playground/Code/Time

 Don't call setSyncProvider to set the time provider. The system time will be used.

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

#include "Time.h"
#include "DateStrings.h"

void digitalClockDisplay();
void printDigits(int digits);

void setup() {

	// initialization
	Serial.begin(9600);	

	// Delay 1 second. Make sure there are at least one second in the system clock.	
	// Otherwise, the now() function will return 0, and the function refreshCache will do nothing.
	// All the data saved in tmElements_t tm will be 0; 
	// If we query the day, month, year this time, they are all zeros, which is wrong.
	// To make the library correct, the initial data of tmElements_t tm should be 0:00:0 Thursday 1 Jan 1970
	delay(1000); 
}

void loop() {

	digitalClockDisplay();  
	delay(5000); // 5 seconds
}

void digitalClockDisplay(){
	// digital clock display of the time
	Serial.print(hour());
	printDigits(minute());
	printDigits(second());
	Serial.print(" ");
	Serial.print(dayStr(weekday()));
	Serial.print(" ");
	Serial.print(day());
	Serial.print(" ");
	Serial.print(monthShortStr(month()));
	Serial.print(" ");
	Serial.print(year()); 
	Serial.println(); 
}

void printDigits(int digits){
	// utility function for digital clock display: prints preceding colon and leading 0
	Serial.print(":");
	if(digits < 10)
		Serial.print('0');
	Serial.print(digits);
}
