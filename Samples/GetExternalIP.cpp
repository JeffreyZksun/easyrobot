
/**************************************************************************************
Functionality:	
Connect a external web server to get the external IP of the router. 
Display the received data in Serial Monitor

Algorithm: The arduino client sends some data to the external web server. The server responses the data with the external IP of this client.
	Arduino gets the external IP by analyzing the received data.

The circuit:
Arduino + Ethernet Shield
Connect to network.
Connect to PC through serial port.

More:

Occupied MCU resource:
<Indicate which resources are occupied by this functionality. 
When try to implement other functionalities, these resources shouldn't be used again.>

Author:		Jeffrey Sun
Created:	2011-7-21

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xCC, 0xAC, 0xBE, 0xEF, 0xFE, 0x92 }; // make sure this is unique on your network

char serverName[] = "www.ip.cn";


// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

#define DATA_BUFFER_SIZE 256
#define IP_BUFFER_SIZE 24
char gDataBuffer[DATA_BUFFER_SIZE];
char gIPBuffer[IP_BUFFER_SIZE]; // 255.255.255.255.255.255\0

int gIpAddress[6] = {0};
bool bIsIPv6 = false;

bool GetIPAddressData();
bool GetIpAddressFromDataBuffer(char* dataBuffer, const int dataBufferSize
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

	delay(2000); // wait 2 seconds.
}

bool GetIPAddressData()
{
	bool bFindIP = false;

	Serial.println("**************[IP]Begin send request**********************");
	Serial.println("Connecting...");
	bool bFail = false;
	// If you get a connection, report back via serial:
	if (client.connect(serverName, 80)) {
		Serial.println("Connected");
		// Make a HTTP request:
		client.println("GET /getip.php?action=getip&ip_url= HTTP/1.1");
		client.println();
	} 
	else {
		
		// If you didn't get a connection to the server:
		Serial.println("Error: Connection failed");

		InitializeEthernet();
		bFail = true;
	}

	if(!bFail)
	{
		Serial.println("");
		Serial.println("Waiting for response...");

		for(int i = 0; i < 5; i++)
		{
			if(client.available())
				break;

			delay(200); // Wait for the server. 200ms
		}

		bool bHasData = client.available();
		if (bHasData)
			Serial.println("Output received data");
		else
			Serial.println("Error: Nothing received.");

		if(bHasData)
		{
			// If there are incoming bytes available 
			// from the server, read them and print them:
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


			Serial.println("Parsing the IP address...");
			bFindIP = GetIpAddressFromDataBuffer(gDataBuffer, pos, gIPBuffer, IP_BUFFER_SIZE);
			if(bFindIP)
			{
				Serial.print("IP address is: ");
				Serial.println(gIPBuffer);
			}
			else
			{
				Serial.println("Error: Fail to get the IP address.");
			}
		}
	}

	Serial.println("");
	Serial.println("[Finish!]");
	Serial.println("");

	return bFindIP;
}

bool GetIpAddressFromDataBuffer(char* dataBuffer, const int dataBufferSize
								, char* ipBuffer, const int ipBufferSize)
{
	if(NULL == dataBuffer || dataBufferSize < 8 
		|| ipBuffer == NULL || ipBufferSize < 8)
		return false;

	/* The received data is like
	HTTP/1.1 200 OK
	Connection: close
	Transfer-Encoding: chunked
	X-Powered-By: PHP/5.2.17-0.dotdeb.0
	content-type: text/html;charset=GB2312
	Date: Thu, 21 Jul 2011 12:22:16 GMT
	Server: lighttpd/1.4.19

	<span class='orange'>124.79.170.185</span>&nbsp;来自：上海市 电信(闵行区)
	*/
	const char* pToken = "orange\'>"; // length = 8
	int tokenLength = 8; // hard code for performance

	char *theFind = strstr( dataBuffer, pToken);

	if ( NULL ==  theFind)
		return false;

	int bufferIndex = theFind - dataBuffer;

	int theIpStart = bufferIndex + tokenLength;
	int ipPos = 0;
	for(; ipPos < ipBufferSize -1 && theIpStart + ipPos < dataBufferSize; ipPos++)
	{
		if(dataBuffer[theIpStart + ipPos] == '<')
			break;

		ipBuffer[ipPos] = dataBuffer[theIpStart + ipPos];
	}
	ipBuffer[ipPos] = '\0';

	return true;
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