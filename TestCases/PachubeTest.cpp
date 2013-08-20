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
#include <Arduino.h>
#include <Ethernet.h>
#include <string.h>
#include <SPI.h>
#include <Time.h>

//#undef int() // needed by arduino 0011 to allow use of stdio
#include <stdio.h> // for function sprintf

#define SHARE_FEED_ID              23408     // this is your Pachube feed ID that you want to share to
#define REMOTE_FEED_ID             256      // this is the ID of the remote Pachube feed that you want to connect to
#define REMOTE_FEED_DATASTREAMS    4        // make sure that remoteSensor array is big enough to fit all the remote data streams

// Rate-limitation for free account: 5 api requests / minute, Import 500 datapoints / day
// send 1 request at least 3 minutes
#define UPDATE_INTERVAL            5    // if the connection is good wait 1000 seconds before updating again - should not be less than 5
#define RESET_INTERVAL             5    // if connection fails/resets wait 1000 seconds before trying again - should not be less than 5

#define PACHUBE_API_KEY            "MsKBIqAa8VoRrR2R2jjJCO6VolYGqzKtpKeQ2FASwfA" // fill in your API key 

byte mac[] = { 0xCC, 0xAC, 0xBE, 0xEF, 0xFE, 0x91 }; // make sure this is unique on your network
byte ip[] = { 192, 168, 1, 177   };                  // no DHCP so we set our own IP address
byte remoteServer[] = { 173,203,98,29 };            // pachube.com

float remoteSensor[REMOTE_FEED_DATASTREAMS];        // we know that feed 256 has floats - this might need changing for feeds without floats


//////////////Ethernte//////////////
Client localClient(remoteServer, 80);
unsigned int interval;

char buff[64];
int pointer = 0;

void setupEthernet();
void resetEthernetShield();
void clean_buffer();

///////////////////////////

/////////////Pachube functions//////////////
char pachube_data[70];

boolean found_status_200 = false;
boolean found_session_id = false;
boolean found_CSV = false;
char *found;
unsigned int successes = 0;
unsigned int failures = 0;
boolean ready_to_update = true;
boolean reading_pachube = false;

boolean request_pause = false;
boolean found_content = false;

unsigned long last_connect;

int content_length;

void pachube_in_out();


void disconnect_pachube();
void checkForResponse();
//////////////////////////

void setup() {

	Serial.begin(9600); 
	setupEthernet(); 

}

void loop() {

	// call 'pachube_in_out' at the beginning of the loop, handles timing, requesting
	// and reading. use serial monitor to view debug messages

	pachube_in_out();
}
//////////////////////////////////////////////////////////////////////////

void setupEthernet(){
	resetEthernetShield();
	Client remoteClient(255);
	delay(500);
	interval = UPDATE_INTERVAL;
	Serial.println("setup complete");
}

void clean_buffer() {
	pointer = 0;
	memset(buff,0,sizeof(buff)); 
}

void resetEthernetShield(){
	Serial.println("reset ethernet");
	Ethernet.begin(mac, ip);
}

//////////////////////////////

void pachube_in_out(){

	if (minute() < last_connect) last_connect = minute();

	if (request_pause){
		if ((minute() - last_connect) > interval){
			ready_to_update = true;
			reading_pachube = false;
			request_pause = false;
			found_status_200 = false;
			found_session_id = false;
			found_CSV = false;

			Serial.print("Ready to connect: ");
			Serial.println(minute());
			Serial.print("interval: ");
			Serial.println(interval);
		}
	}

	if (ready_to_update){
		Serial.println("Connecting...");
		if (localClient.connect()) {

			// here we assign comma-separated values to 'data', which will update Pachube datastreams
			// we use all the analog-in values, but could of course use anything else millis(), digital
			// inputs, etc. . i also like to keep track of successful and failed connection
			// attempts, sometimes useful for determining whether there are major problems.

			sprintf(pachube_data,"%d,%d,%d:%d:%d-%d/%d",analogRead(0),analogRead(1),hour(),minute(),second(),month(),day());
			content_length = strlen(pachube_data);

			Serial.println("GET request to retrieve");

			localClient.print("GET /api/");
			localClient.print(REMOTE_FEED_ID);
			localClient.print(".csv HTTP/1.1\nHost: pachube.com\nX-PachubeApiKey: ");
			localClient.print(PACHUBE_API_KEY);
			localClient.print("\nUser-Agent: Arduino (Pachube In Out v1.1)");
			localClient.println("\n");

			//Serial.println("finished GET now PUT, to update");

			localClient.print("PUT /api/");
			localClient.print(SHARE_FEED_ID);
			localClient.print(".csv HTTP/1.1\nHost: pachube.com\nX-PachubeApiKey: ");
			localClient.print(PACHUBE_API_KEY);

			localClient.print("\nUser-Agent: Arduino (Pachube In Out v1.1)");
			localClient.print("\nContent-Type: text/csv\nContent-Length: ");
			localClient.print(content_length);
			localClient.print("\nConnection: close\n\n");
			localClient.print(pachube_data);



			localClient.print("\n");

			ready_to_update = false;
			reading_pachube = true;
			request_pause = false;
			interval = UPDATE_INTERVAL;

			// Serial.print("finished PUT: ");
			// Serial.println(millis());

		} 
		else {
			Serial.print("connection failed!");
			Serial.print(++failures);
			found_status_200 = false;
			found_session_id = false;
			found_CSV = false;
			ready_to_update = false;
			reading_pachube = false;
			request_pause = true;
			last_connect = minute();
			interval = RESET_INTERVAL;
			setupEthernet();
		}
	}

	while (reading_pachube){
		while (localClient.available()) {
			checkForResponse();
		} 

		if (!localClient.connected()) {
			disconnect_pachube();
		}
	} 
}

void disconnect_pachube(){
	Serial.println("disconnecting.\n=====\n\n");
	localClient.stop();
	ready_to_update = false;
	reading_pachube = false;
	request_pause = true;
	last_connect = minute();
	found_content = false;
	resetEthernetShield();
}


void checkForResponse(){  
	char c = localClient.read();
	Serial.print(c);
	buff[pointer] = c;
	if (pointer < 64) pointer++;
	if (c == '\n') {
		found = strstr(buff, "200 OK");
		if (found != 0){
			found_status_200 = true; 
			//Serial.println("Status 200");
		}
		buff[pointer]=0;
		found_content = true;
		clean_buffer();    
	}

	if ((found_session_id) && (!found_CSV)){
		found = strstr(buff, "HTTP/1.1");
		if (found != 0){
			char csvLine[strlen(buff)-9];
			strncpy (csvLine,buff,strlen(buff)-9);

			//Serial.println("This is the retrieved CSV:");     
			//Serial.println("---");     
			//Serial.println(csvLine);
			//Serial.println("---");   
			Serial.println("\n--- updated: ");
			Serial.println(pachube_data);
			Serial.println("\n--- retrieved: ");
			char delims[] = ",";
			char *result = NULL;
			char * ptr;
			result = strtok_r( buff, delims, &ptr );
			int counter = 0;
			while( result != NULL ) {
				remoteSensor[counter++] = atof(result); 
				result = strtok_r( NULL, delims, &ptr );
			}  
			for (int i = 0; i < REMOTE_FEED_DATASTREAMS; i++){
				Serial.print( (int)remoteSensor[i]); // because we can't print floats
				Serial.print("\t");
			}

			found_CSV = true;

			Serial.print("\nsuccessful updates=");
			Serial.println(++successes);

		}
	}

	if (found_status_200){
		found = strstr(buff, "_id=");
		if (found != 0){
			clean_buffer();
			found_session_id = true; 
		}
	}
}
