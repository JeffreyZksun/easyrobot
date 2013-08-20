/*
Publish the data of the house to internet.

Preparation
 * Make sure the MAX_DATASTREAM_NUM defined in ERxPachube.h no less than 5.
 * Create a feed on the pacube.com and create 5 data streams with the ids 0, 1, 2, 3, 4.

Requried library
	ERxPachube: http://code.google.com/p/pachubelibrary/
	ERxAM2311
	DHT11
	Time

Required hardware
	Arduino
	Ethernetshield
	Sensor DHT11
	Sensor AM2311

Circuit:
* Stack Ethernet shield over the Arduino Board
* Ethernet shield attached to pins 10, 11, 12, 13
* Connect the data pin of DHT11 to Arduino pin 2 (DHT11PIN)
* Connect the SDA and SCL pins of AM2311 to SDA and SCL pins of Arduino
* Connect the VCC and GND pins of DHT11 and AM2311 to +5 and GND pins of Arduino
* Connect the Ethernet shield to the router

* Created 17 December 2011
* By Jeffrey Sun
* http://code.google.com/p/houseonline/

*/
#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxPachube.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Time.h>
#include <ERxAM2311.h>
#include <Wire.h>
#include <DHT11.h>


byte mac[] = { 0xCC, 0xAC, 0xBE, 0xEF, 0xFE, 0x91 }; // make sure this is unique on your network
byte ip[] = { 192, 168, 1, 177   };                  // no DHCP so we set our own IP address

#define PACHUBE_API_KEY				"YourKey" // Fill in your API key PACHUBE_API_KEY
#define PACHUBE_FEED_ID				42131 // Fill in your id

// Rate-limitation for free account: 5 api requests / minute, Import 500 datapoints / day
// send 1 request at least 1 minutes
#define UPDATE_INTERVAL            1 // minutes
unsigned long last_connect = 0;

unsigned int gTotalQuantity = 0;
unsigned int gSuccessQuantity = 0;

#define BUFFER_SIZE 128
char pGlogalDataBuffer[BUFFER_SIZE]; // It is used to get the data from program memory.


ERxPachubeDataOut dataout(PACHUBE_API_KEY, PACHUBE_FEED_ID);

ERxAM2311 amSensor;
dht11 dhtSensor;

#define DHT11PIN 2

void PrintDataStream(const ERxPachube& pachube);

void setup() {
	Serial.begin(9600);
	Ethernet.begin(mac, ip);

	dataout.addData(0); // Temp-1
	dataout.addData(1); // Hum-1
	dataout.addData(2); // Temp-2
	dataout.addData(3); // Hum-2
	dataout.addData(4); // Success rate

	Serial.println("Starting...");

}

void loop() {

	if (minute() < last_connect) 
		last_connect = minute();

	if ((minute() - last_connect) >= UPDATE_INTERVAL)
	{
		last_connect = minute();

		gTotalQuantity++;

		amSensor.read();
		dhtSensor.read(DHT11PIN);

		dataout.updateData(0, amSensor.temperature);
		dataout.updateData(1, amSensor.humidity);
		dataout.updateData(2, dhtSensor.temperature);
		dataout.updateData(3, dhtSensor.humidity);

		sprintf(pGlogalDataBuffer,"%d/%d", gSuccessQuantity, gTotalQuantity);
		dataout.updateData(4, pGlogalDataBuffer);

		int status = dataout.updatePachube();

		if(200 == status)
			gSuccessQuantity++;

		Serial.print("sync status code <OK == 200> => ");
		Serial.println(status);

		PrintDataStream(dataout);
	}
}

void PrintDataStream(const ERxPachube& pachube)
{
	Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++");
	unsigned int count = pachube.countDatastreams();
	Serial.print("data count=> ");
	Serial.println(count);

	Serial.println("<id>,<value>");
	for(unsigned int i = 0; i < count; i++)
	{
		Serial.print(pachube.getIdByIndex(i));
		Serial.print(",");
		Serial.print(pachube.getValueByIndex(i));
		Serial.println();
	}
}
