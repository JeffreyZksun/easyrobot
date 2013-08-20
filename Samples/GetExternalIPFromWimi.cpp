/**************************************************************************************
Functionality:	
Connect a external web server to get the external IP of the router. 
Display the received data in Serial Monitor

Algorithm: The arduino client sends some data to the external web server. The server responses the data with the external IP of this client.
	Arduino gets the external IP by analyzing the received data.


	Use the hurl.it to check the response. The URL is http://automation.whatismyip.com/n09230945.asp
HTTP request
	GET /n09230945.asp HTTP/1.1
	Host: automation.whatismyip.com
	Accept: *//* 

HTTP repsonse
	HTTP/1.1 200 OK
	Date: Sun, 14 Aug 2011 10:09:15 GMT
	Server: Microsoft-IIS/6.0
	X-Powered-By: ASP.NET
	Content-Length: 12
	Content-Type: text/html
	Set-Cookie: ASPSESSIONIDASSDTBBA=BCMKBALDIFIIMKONAJPOHBOD; path=/
	Cache-control: private

	58.38.54.125

From the http response body, we can see it is exactly the ip address we want.

The circuit:
Arduino + Ethernet Shield
Connect to network.
Connect to PC through serial port.

More:

Author:		Jeffrey Sun
Created:	2011-7-21

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetClient.h>

byte mac[] = { 0xCC, 0xAC, 0xBE, 0xEF, 0xFE, 0x92 }; // make sure this is unique on your network

char serverName[] = "automation.whatismyip.com";

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

#define DATA_BUFFER_SIZE 128
#define IP_BUFFER_SIZE 24
char gDataBuffer[DATA_BUFFER_SIZE];
char gIPBuffer[IP_BUFFER_SIZE]; // 255.255.255.255.255.255\0

int gIpAddress[6] = {0};
bool bIsIPv6 = false;

bool GetIPAddressData();
bool GetIpAddressFromDataBuffer(char* httpBodyBuffer, const int httpBodyBufferSize
								, char* ipBuffer, const int ipBufferSize);

void InitializeEthernet();

void setup() {
	// start the serial library:
	Serial.begin(9600);

	InitializeEthernet();

}

void loop()
{
	GetIPAddressData();

	delay(30000); // wait 2 seconds.
}

unsigned int getline(Client& client, char *buffer, unsigned int bufsize);

bool GetIPAddressData()
{
	bool bFindIP = false;

	Serial.println("**************[IP]Begin send request**********************");
	Serial.println("=>Connecting...");
	bool bFail = false;
	// If you get a connection, report back via serial:
	if (client.connect(serverName, 80) > 0) {
		/*
		GET /n09230945.asp HTTP/1.1
		Host:automation.whatismyip.com
		Accept: *//*
		
		*/
		Serial.println("=>Connected");
		// Make a HTTP request:
		client.println("GET /n09230945.asp HTTP/1.1");
		client.println("Host:automation.whatismyip.com");
		client.println("Accept: */*");
		//client.println("Connection: close");
		client.println();
	} 
	else {
		// If you didn't get a connection to the server:
		Serial.println("=>Error: Connection failed. Initialize Ethernet Again.");

		InitializeEthernet();

		bFail = true;
	}

	if(!bFail)
	{
		Serial.println("");
		Serial.println("=>Waiting for response...");

		for(int i = 0; i < 5; i++)
		{
			if(client.available())
				break;

			delay(200); // Wait for the server. 200ms
		}

		bool bHasData = client.available();
		if (bHasData)
			Serial.println("=>Output received data");
		else
			Serial.println("=>Error: Nothing received.(Timeout)");

		if(bHasData)
		{
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

			bool bHttpBodyBegin = false;
			while(client.available())
			{
				int length = getline(client, gDataBuffer, DATA_BUFFER_SIZE);
				Serial.print(gDataBuffer);

				if(length == 2) // This is empty. It means the http header is finished. Next is http body.
				{
					bHttpBodyBegin = true;
					break;
				}
			}

			if(bHttpBodyBegin)
			{
				Serial.println("=>Begin to get http body...");
				// Read the http body
				int pos = 0;
				while (client.available()) 
				{
					if(pos >= (DATA_BUFFER_SIZE - 1))
					{
						Serial.println();
						Serial.println("Warning: The received data is too long. Discard the redundant data.");
						break;
					}

					char c = client.read();
					gDataBuffer[pos] = c;
					pos++;
					Serial.print(c);
				}
				gDataBuffer[pos] = '\0';


				Serial.println("");
				Serial.println("=>Parsing the IP address...");
				bFindIP = GetIpAddressFromDataBuffer(gDataBuffer, pos, gIPBuffer, IP_BUFFER_SIZE);
				if(bFindIP)
				{
					Serial.print("=>Success: IP address is: ");
					Serial.println(gIPBuffer);
				}
				else
				{
					Serial.println("=>Error: Fail to get the IP address.");
				}

				// Close the link every time. Otherwise, the connection can be created only on first time.
				// The following request will fail.
				client.stop();
			}
			else
			{
				Serial.println("=>Error: No http body");
			}
		}
	}

	Serial.println("");
	Serial.println("[Finish!]");
	Serial.println("");

	return bFindIP;
}

bool GetIpAddressFromDataBuffer(char* httpBodyBuffer, const int httpBodyBufferSize
								, char* ipBuffer, const int ipBufferSize)
{
	if(NULL == httpBodyBuffer || httpBodyBufferSize < 8 
		|| ipBuffer == NULL || ipBufferSize < 8)
		return false;

	int ipPos = 0;
	for(; ipPos < ipBufferSize -1; ipPos++)
	{
		ipBuffer[ipPos] = httpBodyBuffer[ipPos];
	}
	ipBuffer[ipPos] = '\0';

	return (ipPos > 8);
}

// Read a line from the data buffer. 
// Return value : length of the line. (not counting the '\0')
unsigned int getline(Client& client, char *buffer, unsigned int bufsize)
{

	unsigned int length = 0;
	char c;
	while (client.available()) {
		c = client.read();
		if (length < bufsize - 1){ // The last one is for '\0'
			buffer[length] = c;
			length ++;
		}
		else
		{
			break; // No buffer available
		}

		if(length >= 2)
		{
			// Note: we can NOT use && here. In some cases, there is only an '\n'.
			if(buffer[length-2] == '\r' || buffer[length-1] == '\n') 
				break; // Line finish
		}
	}

	buffer[length] = '\0';
	return length;
}

void InitializeEthernet()
{
	// start the Ethernet connection:
	int i = 0;
	while(Ethernet.begin(mac) == 0)
	{
		Serial.print(++i);
		Serial.println(": Failed to configure Ethernet using DHCP");
		delay(500);
	}

	// print the local IP address:
	Serial.print("My Local IP address is: ");
	for (byte thisByte = 0; thisByte < 4; thisByte++) {
		// print the value of each byte of the IP address:
		Serial.print(Ethernet.localIP()[thisByte], DEC);
		Serial.print("."); 
	}
	Serial.println();
}
