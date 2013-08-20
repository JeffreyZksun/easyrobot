#include <ERxCore.h>

/**************************************************************************************
Functionality:	
 Send data to and read data from http://www.pachube.com

The circuit:
 Arduino + Ethernet Shield

More:
<Add more information regarding the hardware>

Occupied MCU resource:
<Indicate which resources are occupied by this functionality. 
When try to implement other functionalities, these resources shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============

#include <Ethernet.h>
#include <string.h>
#include <SPI.h>
#include <Time.h>
#include <Udp.h>
#include <ERxPachube.h>
#include <stdio.h> // for function sprintf
#include <avr/pgmspace.h>

#define ENABLE_DEBUG_MESSAGE
#define ENBLE_DATAIN

/*************************Ethernet************************************/
byte mac[] = { 0xCC, 0xAC, 0xBE, 0xEF, 0xFE, 0x91 }; // make sure this is unique on your network
byte ip[] = { 192, 168, 1, 177   };                  // no DHCP so we set our own IP address

/*************************Pachube************************************/
#define SHARE_FEED_ID              23408     // this is your Pachube feed ID that you want to share to
#define REMOTE_FEED_ID             256      // this is the ID of the remote Pachube feed that you want to connect to
#define PACHUBE_API_KEY            "MsKBIqAa8VoRrR2R2jjJCO6VolYGqzKtpKeQ2FASwfA" // fill in your API key 


// Rate-limitation for free account: 5 api requests / minute, Import 500 datapoints / day
// send 1 request at least 5 minutes
#define UPDATE_INTERVAL            1 // minutes

#ifdef ENBLE_DATAIN
ERxPachubeDataIn datain(PACHUBE_API_KEY, REMOTE_FEED_ID);
#endif

ERxPachubeDataOut dataout(PACHUBE_API_KEY, SHARE_FEED_ID);

unsigned long last_connect = 0;

#ifdef ENBLE_DATAIN
int last_datain_http_stattus_code = 0;
#endif

int last_dataout_http_stattus_code = 0;
int request_counter = 0;
int error_datain_counter = 0;
int error_dataout_counter = 0;

#define bufferSIZE 128
char pGlogalDataBuffer[bufferSIZE]; // It is used to get the data from program memory.

#define bufferSIZE2 128
char pGlogalDataBuffer2[bufferSIZE2];

/*************************NTP server************************************/
// NTP server
unsigned int localPort = 8888;	// local port to listen for UDP packets
byte timeServer[]  = { 192, 43, 244, 18}; // time.nist.gov
const int NTP_PACKET_SIZE= 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

#define NTP_TIME_SYNC_INTERVAL 3600 // 3600 seconds

/*************************Get external IP************************************/
/*
Issue 1 - The returned status code is 301 as below

HTTP/1.1 301 Moved Permanently
Date: Mon, 12 Dec 2011 13:10:24 GMT
Server: Microsoft-IIS/6.0
X-Powered-By: ASP.NET
Location: http://www.whatismyip.com/n09230945.asp
Content-Length: 249
Content-type: text/html

Reason: The Ip of automation.whatismyip.com is changed.
Fix: ping the automation.whatismyip.comm and use the new ip instead.
*/
byte _IpServer[] = { 72,233,89,197}; // automation.whatismyip.com

#define IP_BUFFER_SIZE 24
char gIPBuffer[IP_BUFFER_SIZE]; // 255.255.255.255.255.255\0

/******************Save the static strings in program memory***********/
// NTP strings
//
#ifdef ENABLE_DEBUG_MESSAGE
char ntp_sync_fail_onstart[] PROGMEM = "Sync failed during start" ;
char ntp_sync_success_onstart[] PROGMEM = "Sync successfully during start" ;
char ntp_current_time[] PROGMEM = "Current time:";
char ntp_send_request[] PROGMEM = "Sending time sync request to NTP server." ;
char ntp_try_counter[] PROGMEM = "No date data available. Try counter: .";
char ntp_sync_fail[] PROGMEM = "Time sync failed." ;
char ntp_sync_success[] PROGMEM = "Time sync successfully.";
#else
char ntp_sync_fail_onstart[] PROGMEM = "" ;
char ntp_sync_success_onstart[] PROGMEM = "" ;
char ntp_current_time[] PROGMEM = "";
char ntp_send_request[] PROGMEM = "" ;
char ntp_try_counter[] PROGMEM = "";
char ntp_sync_fail[] PROGMEM = "" ;
char ntp_sync_success[] PROGMEM = "";
#endif

// Pachube strings
#ifdef ENABLE_DEBUG_MESSAGE
char pachube_datain_header[] PROGMEM = "+++++++++++++++++++datain++++++++++++++++++++++++++++++" ;
char pachube_dataout_header[] PROGMEM = "+++++++++++++++++++dataout++++++++++++++++++++++++++++++" ;
char pachube_request_status[] PROGMEM = "request status code <OK == 200> => " ;
char pachube_data_count[] PROGMEM = "data count=> ";
char pachube_id_value_pair[] PROGMEM = "<id>,<value>" ;
#else
char pachube_datain_header[] PROGMEM = "" ;
char pachube_dataout_header[] PROGMEM = "" ;
char pachube_request_status[] PROGMEM = "" ;
char pachube_data_count[] PROGMEM = "";
char pachube_id_value_pair[] PROGMEM = "" ;
#endif

// Get external server settings
#ifdef ENABLE_DEBUG_MESSAGE
char ip_begin_send_request[] PROGMEM = "**************[IP]Begin send request**********************" ;
char ip_connecting[] PROGMEM = "=>Connecting..." ;
char ip_connected[] PROGMEM = "=>Connected" ;
char ip_error_connection_failed[] PROGMEM = "=>Error: Connection failed";
char ip_waiting_for_response[] PROGMEM = "=>Waiting for response..." ;
char ip_output_received_data[] PROGMEM = "=>Output received data" ;
char ip_error_nothing_received[] PROGMEM = "=>Error: Nothing received." ;
char ip_begin_http_body[] PROGMEM = "=>Begin to get http body..." ;
char ip_warning_data_overflow[] PROGMEM = "Warning: The received data is too long. Discard the redundant data." ;
char ip_parse_ip[] PROGMEM = "=>Parsing the IP address..." ;
char ip_success[] PROGMEM = "=>Success: IP address is: " ;
char ip_error_fail_to_get_ip[] PROGMEM = "=>Error: Fail to get the IP address." ;
char ip_error_no_http_body[] PROGMEM = "=>Error: No http body" ;
char ip_finish[] PROGMEM = "[Finish!]" ;
#else
char ip_begin_send_request[] PROGMEM = "" ;
char ip_connecting[] PROGMEM = "" ;
char ip_connected[] PROGMEM = "" ;
char ip_error_connection_failed[] PROGMEM = "";
char ip_waiting_for_response[] PROGMEM = "" ;
char ip_output_received_data[] PROGMEM = "" ;
char ip_error_nothing_received[] PROGMEM = "" ;
char ip_begin_http_body[] PROGMEM = "" ;
char ip_warning_data_overflow[] PROGMEM = "" ;
char ip_parse_ip[] PROGMEM = "" ;
char ip_success[] PROGMEM = "" ;
char ip_error_fail_to_get_ip[] PROGMEM = "" ;
char ip_error_no_http_body[] PROGMEM = "" ;
char ip_finish[] PROGMEM = "" ;
#endif

/***************************************************************/
// Declaration

void pachube_in_out();
void PrintDataStream(const ERxPachube& pachube);
//void PrintString(const String& message) ;

unsigned long getNtpTime();
unsigned long sendNTPpacket(byte *address);

void debug_print_p(const char* pProgMemAddr) ;
void debug_println_p(const char* pProgMemAddr) ;
void debug_println(const char* pMemAddr);
void debug_print(const char* pMemAddr);
void debug_print(const char c);
void debug_println(const int i) ;

void resetEthernetShield();

bool GetIPAddressData();
//
//
void setup() {

	Serial.begin(9600); 
	Serial.println("setup");

	Ethernet.begin(mac, ip);

	dataout.addData(0);
	dataout.addData(1);
	dataout.addData(2);
	dataout.addData(3);

	setSyncInterval(NTP_TIME_SYNC_INTERVAL);
	setSyncProvider(getNtpTime);

	if(timeStatus()== timeNotSet)
	{
		debug_println_p(ntp_sync_fail_onstart/*"Sync failed during start"*/);
	}
	else
	{
		debug_println_p(ntp_sync_success_onstart/*"Sync successfully during start"*/);
		char currentTime[50];
		sprintf(currentTime,"%d-%d-%d %d:%d:%d", year(),month(),day(),hour(),minute(),second());
		debug_print_p(ntp_current_time/*"Current time:"*/);
		debug_println(currentTime);
	}

	gIPBuffer[0] = '\0';
}

void loop() {

	pachube_in_out();
}
//////////////////////////////////////////////////////////////////////////

void pachube_in_out(){

	if (minute() < last_connect) 
		last_connect = minute();

		if ((minute() - last_connect) >= UPDATE_INTERVAL)
		{
			int current_status  = 200;
#ifdef ENBLE_DATAIN
			debug_println_p(pachube_datain_header/*"+++++++++++++++++++datain++++++++++++++++++++++++++++++"*/);
			current_status = datain.syncPachube();

			debug_print_p(pachube_request_status/*"request status code <OK == 200> => "*/);
			Serial.println(current_status);

			PrintDataStream(datain);
#endif

			debug_println_p(pachube_dataout_header/*"+++++++++++++++++++dataout++++++++++++++++++++++++++++++"*/);

			char pBuffer[50];
			dataout.updateData(0, analogRead(0));

#ifdef ENBLE_DATAIN
			sprintf(pBuffer,"%d; %d; %d -- %d; %d"
				, error_datain_counter
				, error_dataout_counter
				, request_counter
				, last_datain_http_stattus_code
				, last_dataout_http_stattus_code);
#else
			sprintf(pBuffer,"0; %d; %d -- 200; %d"
				, error_dataout_counter
				, request_counter
				, last_dataout_http_stattus_code);
#endif
			dataout.updateData(1, pBuffer);
			sprintf(pBuffer,"%d-%d-%d %d:%d:%d", year(),month(),day(),hour(),minute(),second());
			dataout.updateData(2, pBuffer);
			if(GetIPAddressData())
			{
				dataout.updateData(3, gIPBuffer);
			}
			last_dataout_http_stattus_code = dataout.updatePachube();

			debug_print_p(pachube_request_status/*"request status code <OK == 200> => "*/);
			Serial.println(last_dataout_http_stattus_code);

			PrintDataStream(dataout);

			last_connect = minute();

#ifdef ENBLE_DATAIN
			// counter the status
			last_datain_http_stattus_code = current_status;

			if(last_datain_http_stattus_code != 200)
				error_datain_counter++;
#endif

			if(last_dataout_http_stattus_code != 200)
				error_dataout_counter++;

			request_counter++;

			// Reset ethernet when encounter error.
#ifdef ENBLE_DATAIN
			if(last_datain_http_stattus_code != 200 || last_dataout_http_stattus_code != 200)
				resetEthernetShield();
#else
			if(last_dataout_http_stattus_code != 200)
				resetEthernetShield();
#endif
		}
}

void PrintDataStream(const ERxPachube& pachube)
{
	unsigned int count = pachube.countDatastreams();
	debug_print_p(pachube_data_count/*"data count=> "*/);
	Serial.println(count);

	debug_println_p(pachube_id_value_pair/*"<id>,<value>"*/);
	for(unsigned int i = 0; i < count; i++)
	{
		Serial.print(pachube.getIdByIndex(i));
		Serial.print(",");
		Serial.print(pachube.getValueByIndex(i));
		Serial.println();
	}
}

//void PrintString(const String& message) 
//{
//	String tmpStr = message;
//	int bufferSize = tmpStr.length() +1;
//	char pBuffer[bufferSize];
//	tmpStr.toCharArray(pBuffer, bufferSize);
//
//	Serial.print(pBuffer); 
//}

/*-------- Ethernet code ----------*/

void resetEthernetShield()
{
	Ethernet.begin(mac, ip);
}

/*-------- NTP code ----------*/

unsigned long getNtpTime()
{
	// We need to call the begin to reset the socket.
	// Because localClient.connect() may occupy the same socket.
	Udp.begin(localPort);

	debug_println_p(ntp_send_request/*"Sending time sync request to NTP server."*/);
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

			debug_println_p(ntp_sync_success/*"Time sync successfully."*/);

			return epoch;
		}

		debug_println_p(ntp_try_counter/*"No date data available. Try counter: ."*/);
		debug_println(tryCounter++);
		delay(100);
	}

	debug_println_p(ntp_sync_fail/*"Time sync failed."*/);

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

/*******************Get external IP address*****************************/

// http://www.whatismyip.com/faq/automation.asp
// The external IP can be returned from the page http://automation.whatismyip.com/n09230945.asp
// Access limitation
// We ask that automated files hit our site no more then once every five minutes or once every 300 seconds.

int previous_ip_access = -10;

unsigned int getline(Client& client, char *buffer, unsigned int bufsize); // Defined in ERxPachube.cpp

bool GetIpAddressFromDataBuffer(char* httpBodyBuffer, const int httpBodyBufferSize
								, char* ipBuffer, const int ipBufferSize);

bool GetIPAddressData()
{
	if((minute() - previous_ip_access) < 7)
		return false;

	previous_ip_access = minute();

	Client client(_IpServer, 80);

	bool bFindIP = false;

	debug_println_p(ip_begin_send_request);
	debug_println_p(ip_connecting);
	bool bFail = false;
	// If you get a connection, report back via serial:
	if (client.connect()) {
		debug_println_p(ip_connected);
		// Make a HTTP request:
		client.println("GET /n09230945.asp HTTP/1.1");
		client.println("Host: automation.whatismyip.com");
		client.println("Accept: */*");
		client.println();
	} 
	else {
		Ethernet.begin(mac, ip);
		// If you didn't get a connection to the server:
		debug_println_p(ip_error_connection_failed);
		bFail = true;
	}

	if(!bFail)
	{
		debug_println("");
		debug_println_p(ip_waiting_for_response);

		for(int i = 0; i < 5; i++)
		{
			if(client.available())
				break;

			delay(200); // Wait for the server. 200ms
		}

		bool bHasData = client.available();
		if (bHasData)
			debug_println_p(ip_output_received_data);
		else
			debug_println_p(ip_error_nothing_received);

		if(bHasData)
		{
			bool bHttpBodyBegin = false;
			while(client.available())
			{
				int length = getline(client, pGlogalDataBuffer2, bufferSIZE2);
				debug_print(pGlogalDataBuffer2);

				if(length == 2) // This is empty. It means the http header is finished. Next is http body.
				{
					bHttpBodyBegin = true;
					break;
				}
			}

			if(bHttpBodyBegin)
			{
				debug_println_p(ip_begin_http_body);
				// Read the http body
				int pos = 0;
				while (client.available()) 
				{
					if(pos >= (bufferSIZE2 - 1))
					{
						debug_println("");
						debug_println_p(ip_warning_data_overflow);
						break;
					}

					char c = client.read();
					pGlogalDataBuffer2[pos] = c;
					pos++;
					debug_print(c);
				}
				pGlogalDataBuffer2[pos] = '\0';


				debug_println("");
				debug_println_p(ip_parse_ip);
				bFindIP = GetIpAddressFromDataBuffer(pGlogalDataBuffer2, pos, gIPBuffer, IP_BUFFER_SIZE);
				if(bFindIP)
				{
					debug_print_p(ip_success);
					debug_println(gIPBuffer);
				}
				else
				{
					debug_println_p(ip_error_fail_to_get_ip);
				}
			}
			else
			{
				debug_println_p(ip_error_no_http_body);
			}
		}
	}

	debug_println("");
	debug_println_p(ip_finish);
	debug_println("");

	return bFindIP;
}

bool GetIpAddressFromDataBuffer(char* httpBodyBuffer, const int httpBodyBufferSize
								, char* ipBuffer, const int ipBufferSize)
{
	if(NULL == httpBodyBuffer || httpBodyBufferSize < 8 
		|| ipBuffer == NULL || ipBufferSize < 8)
		return false;

	/* The received data is like
	HTTP/1.1 200 OK
	Date: Sun, 14 Aug 2011 10:05:57 GMT
	Server: Microsoft-IIS/6.0
	X-Powered-By: ASP.NET
	Content-Length: 14
	Content-Type: text/html
	Set-Cookie: ASPSESSIONIDCQTCQDBB=KCDOBALDODNOIPGMIHCHDIDE; path=/
	Cache-control: private

	58.38.54.125
	*/
	int ipPos = 0;
	for(; ipPos < ipBufferSize -1; ipPos++)
	{
		ipBuffer[ipPos] = httpBodyBuffer[ipPos];
	}
	ipBuffer[ipPos] = '\0';

	return (ipPos > 8);
}

/*******************Program memory*****************************/

void loadStringFromProgramMemory(char* pBuffer, const char* pProgMemAddr, unsigned int size); // Defined in RExPachube.cpp

void debug_println_p(const char* pProgMemAddr) 
{
#ifdef ENABLE_DEBUG_MESSAGE
	loadStringFromProgramMemory(pGlogalDataBuffer, pProgMemAddr, bufferSIZE);

	Serial.println(pGlogalDataBuffer); 
#endif
}

void debug_print_p(const char* pProgMemAddr) 
{
#ifdef ENABLE_DEBUG_MESSAGE
	loadStringFromProgramMemory(pGlogalDataBuffer, pProgMemAddr, bufferSIZE);

	Serial.print(pGlogalDataBuffer);
#endif
}

void debug_println(const char* pMemAddr) 
{
#ifdef ENABLE_DEBUG_MESSAGE

	Serial.println(pMemAddr); 
#endif
}

void debug_print(const char* pMemAddr) 
{
#ifdef ENABLE_DEBUG_MESSAGE

	Serial.print(pMemAddr); 
#endif
}

void debug_print(const char c) 
{
#ifdef ENABLE_DEBUG_MESSAGE

	Serial.print(c); 
#endif
}

void debug_println(const int i) 
{
#ifdef ENABLE_DEBUG_MESSAGE

	Serial.print(i); 
#endif
}