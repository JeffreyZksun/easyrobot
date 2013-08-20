#include <ERxCore.h>

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

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192,168,1,177 };
byte server[] = { 119,75,217,56 }; // www.baidu.com

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
Client client(server, 80);

void setup() {
	// start the Ethernet connection:
	Ethernet.begin(mac, ip);
	// start the serial library:
	Serial.begin(9600);
	// give the Ethernet shield a second to initialize:
	delay(1000);
	Serial.println("connecting...");

	// if you get a connection, report back via serial:
	if (client.connect()) {
		Serial.println("connected");
		// Make a HTTP request:
		client.println("GET /s?wd=arduino HTTP/1.0");
		client.println();
	} 
	else {
		// if you didn't get a connection to the server:
		Serial.println("connection failed");
	}
}

void loop()
{
	// if there are incoming bytes available 
	// from the server, read them and print them:
	if (client.available()) {
		char c = client.read();
		Serial.print(c);
	}

	// if the server's disconnected, stop the client:
	if (!client.connected()) {
		Serial.println();
		Serial.println("disconnecting.");
		client.stop();

		// do nothing forevermore:
		for(;;)
			;
	}
}