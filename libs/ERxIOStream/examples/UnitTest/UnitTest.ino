/*
ERxOutputStream unit test

This sketch is used to test ERxIOStream 

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Verfiy the result in the PC Serial monitor.


* Created 31 May 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxIOStream.h>

void RunUnitTest();

void setup() {

	Serial.begin(9600);


	RunUnitTest();
}

void loop() {

	// Do nothing.
}

//*********************Test framework***************//
// To make the unit test code self-constrained, 
// we copy the test fremework code in each unit test other than depends on a unit test library.
int tatalCases = 0;
int succCases = 0;
int failCases = 0;

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

void EqualTest(const char* pCaseName, int actualValue, int expctedValue)
{
	tatalCases++;
	
	OutPutCompareMessage(pCaseName, actualValue == expctedValue);
}

void EqualTest(const char* pCaseName, float actualValue, float expctedValue)
{
	tatalCases++;
	
	float diff = actualValue - expctedValue;
	OutPutCompareMessage(pCaseName, (diff < 1e-6) && (diff > -1e-6));
}
//*************************************************//


//*********************Test case******************//

void RunUnitTest()
{
	BegainUnitTest();
	
	{
		Serial.println("=>Test basic flow");

		unsigned char streamBuffer[64];
		ERxIOStream iostream(streamBuffer, 64, 0);

		EqualTest("Initial size", iostream.available(), 0);
		EqualTest("rdbuf", (int)iostream.rdbuf(), (int)(&streamBuffer));

		size_t write_size = iostream.print("123");
		EqualTest("write_size", write_size, 3);
		EqualTest("size", iostream.available(), 3);
		EqualTest("data 0", streamBuffer[0], '1');
		EqualTest("data 1", streamBuffer[1], '2');
		EqualTest("data 2", streamBuffer[2], '3');

		unsigned long data = iostream.parseULong();
		EqualTest("parseULong", (int)data, 123);
		EqualTest("Post read size", iostream.available(), 0);

		iostream.resetIterator(0, 3);
		EqualTest("resetIterator size", iostream.available(), 3);

		data = iostream.parseULong();
		EqualTest("parseULong", (int)data, 123);
		EqualTest("Post read size", iostream.available(), 0);
	}
	{
		Serial.println("=>Test overflow");

		unsigned char streamBuffer[2];
		ERxIOStream iostream(streamBuffer, 2, 0);

		size_t write_size = iostream.print("123");
		EqualTest("write_size", write_size, 2);

		iostream.flush();
		EqualTest("Post flush size", iostream.available(), 0);

		write_size = iostream.print("123");
		EqualTest("write_size", write_size, 2);
	}

	{
		Serial.println("=>Test underflow");

		unsigned char streamBuffer[64];
		ERxIOStream iostream(streamBuffer, 64, 0);

		size_t write_size = iostream.print("012");
		EqualTest("read 0", '0', iostream.read());
		EqualTest("read 1", '1', iostream.read());
		EqualTest("read 2", '2', iostream.read());
		EqualTest("read 3", -1, iostream.read());
	}

	{
		Serial.println("=>Test peek");

		unsigned char streamBuffer[64];
		ERxIOStream iostream(streamBuffer, 64, 0);

		size_t write_size = iostream.print("012");
		EqualTest("size", iostream.available(), 3);
		EqualTest("peek 0", '0', iostream.peek());
		EqualTest("peek 1", '0', iostream.peek());
		EqualTest("size", iostream.available(), 3);
	}

	{
		Serial.println("=>Test peek");

		unsigned char streamBuffer[64];
		ERxIOStream iostream(streamBuffer, 64, 0);

		size_t write_size = iostream.print("0xFF");
		unsigned long data = iostream.parseULong();
		EqualTest("parseULong", (int)data, 255);
	}

	{
		Serial.println("=>Test print and parse float type");

		unsigned char streamBuffer[64];
		ERxIOStream iostream(streamBuffer, 64, 0);

		const float data = 12.657896;
		size_t write_size = iostream.print(data, 2);
		EqualTest("write_size", write_size, 5);
		EqualTest("size", iostream.available(), 5);
		EqualTest("data 0", streamBuffer[0], '1');
		EqualTest("data 1", streamBuffer[1], '2');
		EqualTest("data 2", streamBuffer[2], '.');
		EqualTest("data 3", streamBuffer[3], '6');
		EqualTest("data 4", streamBuffer[4], '6');

		iostream.print(' ');
		iostream.print(data, 2);
		EqualTest("size", iostream.available(), 11);
		EqualTest("data 6", streamBuffer[6], '1');
		EqualTest("data 7", streamBuffer[7], '2');

		float parsedData = iostream.parseFloat();
		EqualTest("parseFloat", parsedData, 12.66);

		parsedData = iostream.parseFloat();
		EqualTest("parseFloat", parsedData, 12.66);
	}

	// Issue: when the char pointer is NULL, the print function still write the data. The expection is write nothing.
	//{
	//	Serial.println("=>Test print NULL");

	//	unsigned char streamBuffer[64];
	//	ERxIOStream iostream(streamBuffer, 64, 0);

	//	char* pData = "a";
	//	size_t write_size = iostream.print(pData);
	//			Serial.println(write_size);
	//			Serial.println(streamBuffer[0]);
	//	EqualTest("write_size", write_size, 0);
	//	EqualTest("size", iostream.available(), 1);

	//	char* pData2 = NULL;
	//	write_size = iostream.write(pData2);
	//	Serial.println(write_size);
	//	Serial.println(streamBuffer[1]);
	//	EqualTest("write_size", write_size, 0);
	//	EqualTest("size", iostream.available(), 2);
	//}

	
	EndUnitTest();
}