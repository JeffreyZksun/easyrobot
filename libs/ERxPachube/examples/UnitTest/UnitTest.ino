/*
Pachube unit test

This sketch is used to test if the data process logic works correctly. 
It doesn't require connect the Arduino to Ethernet.

* Created 17 December 2011
* By Jeffrey Sun
* http://code.google.com/p/pachubelibrary/

*/

//#define CONNECT_ETHERNET	// If want to test the Ethernet communication, uncomment this macro.

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxPachube.h>

#ifdef CONNECT_ETHERNET
#include <Ethernet.h>
#include <SPI.h>

byte mac[] = { 0xCC, 0xAC, 0xBE, 0xEF, 0xFE, 0x91 }; // make sure this is unique on your network
byte ip[] = { 192, 168, 1, 177   };                  // no DHCP so we set our own IP address
#endif

// For the normal testing
#define PACHUBE_API_KEY				"8VoKMmqj23MsAaO6VolYGqzKeQ2tpKFASwfARrR2R2j" // fill in your API key 
#define PACHUBE_FEED_ID				23408
#define PACHUBE_FEED_ID_STR			"23408"

// For boundary testing and overflow testing 
#define PACHUBE_API_KEY_63			"012345678901234567890123456789012345678901234567890123456789012"
#define PACHUBE_API_KEY_64			"0123456789012345678901234567890123456789012345678901234567890123"
#define PACHUBE_API_KEY_70			"0123456789012345678901234567890123456789012345678901234567890123456789"
#define PACHUBE_FEED_ID_STR_9		"012345678"
#define PACHUBE_FEED_ID_STR_10		"0123456789"
#define PACHUBE_FEED_ID_STR_12		"012345678901"

int tatalCases = 0;
int succCases = 0;
int failCases = 0;

void PrintDataStream(const ERxPachube& pachube);

void BegainUnitTest()
{
	Serial.println("================Begin=================");
	tatalCases = 0;
	succCases = 0;
	failCases = 0;
}

void EndUnitTest()
{
	Serial.println();
	Serial.println("================Finish================");
	Serial.print("TOTAL  : ");
	Serial.println(tatalCases);
	Serial.print("SUCCESS: ");
	Serial.println(succCases);
	Serial.print("FAIL   : ");
	Serial.println(failCases);
}

void OutPutCompareMessage(const char* pCaseName, bool bSucc)
{
	Serial.print(pCaseName);
	Serial.print("	==>	");

	if(bSucc)
	{
		succCases++;
		Serial.print("Success");
	}
	else
	{
		failCases++;
		Serial.print("Fail");
	}

	Serial.println();
}

void EqualTest(const char* pCaseName, const char* actualValue, const char* expctedValue)
{
	tatalCases++;


	OutPutCompareMessage(pCaseName, strcmp(actualValue, expctedValue) == 0);
}

void EqualTest(const char* pCaseName, int actualValue, int expctedValue)
{
	tatalCases++;
	
	OutPutCompareMessage(pCaseName, actualValue == expctedValue);
}

void EqualTest(const char* pCaseName, float actualValue, float expctedValue)
{
	tatalCases++;

	float diff = actualValue - expctedValue;
	OutPutCompareMessage(pCaseName, diff > -1e-6 && diff < 1e-6);
}

void EqualTest(const char* pCaseName, const String& actualValue, const String& expctedValue)
{
	tatalCases++;
	OutPutCompareMessage(pCaseName, actualValue == expctedValue);
}

void RunPachubeUnitTest()
{
	BegainUnitTest();

	{
		ERxPachubeDataOut dataOut(PACHUBE_API_KEY, PACHUBE_FEED_ID_STR);
		EqualTest("getAPIKey", PACHUBE_API_KEY, dataOut.getAPIKey());
		EqualTest("getFeedId", PACHUBE_FEED_ID_STR, dataOut.getFeedId());

		EqualTest("countDatastreams", 0, dataOut.countDatastreams());

		// Id 10
		dataOut.addData(10);
		dataOut.updateData(10, 250);

		EqualTest("countDatastreams", 1, dataOut.countDatastreams());
		EqualTest("getValueInt 10", 250, dataOut.getValueInt(10));
		EqualTest("getValueString 10", String("250"), dataOut.getValueString(10));
		EqualTest("getValueFloat 10", 250.0, dataOut.getValueFloat(10));

		// Id 0
		dataOut.addData(0);
		dataOut.updateData(0, 222);

		EqualTest("countDatastreams", 2, dataOut.countDatastreams());
		EqualTest("getValueInt 0", 222, dataOut.getValueInt(0));

		dataOut.updateData(1, 110);
		EqualTest("getValueInt 1", 0, dataOut.getValueInt(1));

		dataOut.updateData(0, "123");
		EqualTest("getValueInt 0", 123, dataOut.getValueInt(0));
		EqualTest("getValueString 0", String("123"), dataOut.getValueString(0));

		dataOut.updateData(0, "12.3");
		EqualTest("getValueFloat 0", 12.3, dataOut.getValueFloat(0));
		EqualTest("getValueString 0", String("12.3"), dataOut.getValueString(0));


		EqualTest("getValueByIndex 0", String("250"), dataOut.getValueByIndex(0));
		EqualTest("getIdByIndex 0", 10, (int)dataOut.getIdByIndex(0));

		// Id 3
		float fSensorValue = 11.5;
		dataOut.addData(3);
		dataOut.updateData(3, fSensorValue);
		EqualTest("float data updateData", fSensorValue, dataOut.getValueFloat(3));

	#ifdef CONNECT_ETHERNET
		int outStatus = dataOut.updatePachube();
		EqualTest("dataOut.updatePachube", outStatus , 200);
	#endif 

		PrintDataStream(dataOut);

	}


	{
		ERxPachubeDataIn datain(PACHUBE_API_KEY, PACHUBE_FEED_ID_STR);

		EqualTest("getAPIKey", PACHUBE_API_KEY, datain.getAPIKey());
		EqualTest("getFeedId", PACHUBE_FEED_ID_STR, datain.getFeedId());

	#ifdef CONNECT_ETHERNET
		int inStatus = datain.syncPachube();
		EqualTest("datain.syncPachube", inStatus , 200);
	#endif 

	PrintDataStream(datain);


	}

	// API key and Feed ID backward compatabilty testing
	{
		ERxPachubeDataOut dataOut(PACHUBE_API_KEY, PACHUBE_FEED_ID);
		EqualTest("getAPIKey", PACHUBE_API_KEY, dataOut.getAPIKey());
		EqualTest("getFeedId", PACHUBE_FEED_ID_STR, dataOut.getFeedId());
	}
	// API key and Feed ID Boundary testing
	{
		ERxPachubeDataOut dataOut(PACHUBE_API_KEY_63, PACHUBE_FEED_ID_STR_9);
		EqualTest("getAPIKey", PACHUBE_API_KEY_63, dataOut.getAPIKey());
		EqualTest("getFeedId", PACHUBE_FEED_ID_STR_9, dataOut.getFeedId());
	}

	// API key and Feed ID Overflow testing
	{
		ERxPachubeDataIn datain(PACHUBE_API_KEY_64, PACHUBE_FEED_ID_STR_10);

		EqualTest("getAPIKey", PACHUBE_API_KEY_63, datain.getAPIKey());
		EqualTest("getFeedId", PACHUBE_FEED_ID_STR_9, datain.getFeedId());
	}

	{
		ERxPachubeDataOut dataOut(PACHUBE_API_KEY_70, PACHUBE_FEED_ID_STR_12);
		EqualTest("getAPIKey", PACHUBE_API_KEY_63, dataOut.getAPIKey());
		EqualTest("getFeedId", PACHUBE_FEED_ID_STR_9, dataOut.getFeedId());
	}


	EndUnitTest();
}

void setup() {

	Serial.begin(9600);

#ifdef CONNECT_ETHERNET
	Ethernet.begin(mac, ip);
#endif

	RunPachubeUnitTest();
}

void loop() {

	// Do nothing.
}

void PrintDataStream(const ERxPachube& pachube)
{
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
