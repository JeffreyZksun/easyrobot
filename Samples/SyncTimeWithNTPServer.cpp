#include <ERxCore.h>

/**************************************************************************************
Functionality:	
 Sync time with NTP server.
 NTP server : time.nist.gov 
 UDP port : 123

 
The circuit:
<Describe the used circuit connection.>

More:
If Arduino can't access the Ethernet, it will hang when send the udp code (Udp.sendPacket).

Occupied MCU resource:
<Indicate which resources are occupied by this functionality. 
When try to implement other functionalities, these resources shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============

#include <Time.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Udp.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = {  192, 168, 1, 44 }; // set the IP address to an unused address on your network

unsigned int localPort = 8888;	// local port to listen for UDP packets

byte timeServer[]  = { 192, 43, 244, 18}; // time.nist.gov

const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

time_t prevDisplay = 0; // when the digital clock was displayed

void digitalClockDisplay();
void printDigits(int digits);

unsigned long getNtpTime();
unsigned long sendNTPpacket(byte *address);

void setup()
{
	// start Ethernet and UDP
	Ethernet.begin(mac,ip);
	Udp.begin(localPort);
	Serial.begin(9600);
	Serial.println("waiting for sync");

	setSyncInterval(15); // sync every 15 seconds
	setSyncProvider(getNtpTime);

	while(timeStatus()== timeNotSet)
		; // wait until the time is set by the sync provider

	Serial.println("Sync successfully");
}

void loop()
{

	if( now() != prevDisplay) //update the display only if the time has changed
	{
		prevDisplay = now();
		digitalClockDisplay();		
	}
}

void digitalClockDisplay(){
	// digital clock display of the time
	Serial.print(hour());
	printDigits(minute());
	printDigits(second());
	Serial.print(" ");
	Serial.print(year());
	Serial.print("-");
	Serial.print(month());
	Serial.print("-");
	Serial.print(day());
	Serial.println();
}

void printDigits(int digits){
	// utility function for digital clock display: prints preceding colon and leading 0
	Serial.print(":");
	if(digits < 10)
		Serial.print('0');
	Serial.print(digits);
}

/*-------- NTP code ----------*/

unsigned long getNtpTime()
{
	Serial.println("Sending time sync request to NTP server.");
	sendNTPpacket(timeServer); // send an NTP packet to a time server

	unsigned long startMillis = millis();
	int tryCounter = 1;
	while( millis() - startMillis < 1000)  // wait up to one second for the response
	{  // wait to see if a reply is available
		if ( Udp.available() )
		{
			Udp.readPacket(packetBuffer,NTP_PACKET_SIZE);  // read the packet into the buffer

			//the timestamp starts at byte 40 of the received packet and is four bytes,
			// or two words, long. First, esxtract the two words:
			unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
			unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
			// combine the four bytes (two words) into a long integer
			// this is NTP time (seconds since Jan 1 1900):
			unsigned long secsSince1900 = highWord << 16 | lowWord;
			// now convert NTP time into Arduino Time format:
			// Time starts on Jan 1 1970. In seconds, that's 2208988800:
			const unsigned long seventyYears = 2208988800UL;
			// subtract seventy years:
			unsigned long epoch = secsSince1900 - seventyYears;

			Serial.println("Time sync successfully.");

			return epoch;
		}

		Serial.print("No date data available. Try counter: .");
		Serial.println(tryCounter++);
		delay(100);
	}

	Serial.println("Time sync failed.");

	return 0;   // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(byte *address)
{
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	Udp.sendPacket( packetBuffer,NTP_PACKET_SIZE,  address, 123); //NTP requests are to port 123
} 
