#include <ERxCore.h>

/**************************************************************************************
Functionality:	
The device will sleep when char 'S' is received or live for 10s.
The device can be waked up by the UART data.
The led linked to pin 13 can indicate the device status. It is on when the device is sleep.
and is off when the device is awake.

The code is from http://rubenlaguna.com/wp/2008/10/15/arduino-sleep-mode-waking-up-when-receiving-data-on-the-usart/

http://www.arduino.cc/playground/Learning/ArduinoSleepCode

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
#include <avr/power.h>
#include <avr/sleep.h>

int count = 0;                   // counter
#define ledPin 13

void setup()
{
	Serial.begin(9600);
	pinMode(ledPin, OUTPUT);
	digitalWrite(ledPin, LOW);
}

void sleepNow()
{
	/* Now is the time to set the sleep mode. In the Atmega8 datasheet
	* http://www.atmel.com/dyn/resources/prod_documents/doc2486.pdf on page 35
	* there is a list of sleep modes which explains which clocks and 
	* wake up sources are available in which sleep modus.
	*
	* In the avr/sleep.h file, the call names of these sleep modus are to be found:
	*
	* The 5 different modes are:
	*     SLEEP_MODE_IDLE         -the least power savings 
	*     SLEEP_MODE_ADC
	*     SLEEP_MODE_PWR_SAVE
	*     SLEEP_MODE_STANDBY
	*     SLEEP_MODE_PWR_DOWN     -the most power savings
	*
	*  the power reduction management <avr/power.h>  is described in 
	*  http://www.nongnu.org/avr-libc/user-manual/group__avr__power.html
	*/  

	digitalWrite(ledPin, HIGH);

	set_sleep_mode(SLEEP_MODE_IDLE);   // sleep mode is set here

	sleep_enable();          // enables the sleep bit in the mcucr register
	// so sleep is possible. just a safety pin 

	power_adc_disable();
	power_spi_disable();
	power_timer0_disable();
	power_timer1_disable();
	power_timer2_disable();
	power_twi_disable();


	sleep_mode();            // here the device is actually put to sleep!!

	// THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
	sleep_disable();         // first thing after waking from sleep:
	// disable sleep...

	power_all_enable();

	digitalWrite(ledPin, LOW);
	Serial.println("Waking up now.");
}


void loop()
{
	// display information about the counter
	Serial.print("Awake for ");
	Serial.print(count);
	Serial.println("sec");
	count++;
	delay(1000);                           // waits for a second

	// compute the serial input
	if (Serial.available()) {
		int val = Serial.read();
		if (val == 'S') {
			Serial.println("Serial: Entering Sleep mode");
			delay(100);     // this delay is needed, the sleep 
			//function will provoke a Serial error otherwise!!
			count = 0;
			sleepNow();     // sleep function called here
		}
		if (val == 'A') {
			Serial.println("Hola Caracola"); // classic dummy message
		}
	}

	// check if it should go asleep because of time
	if (count >= 10) {
		Serial.println("Timer: Entering Sleep mode");
		delay(100);     // this delay is needed, the sleep 
		//function will provoke a Serial error otherwise!!
		count = 0;
		sleepNow();     // sleep function called here
	}
}

