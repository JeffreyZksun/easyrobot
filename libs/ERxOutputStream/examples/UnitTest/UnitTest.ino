/*
ERxOutputStream unit test

This sketch is used to test ERxOutputStream and its children classes. 

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Verfiy the result in the PC Serial monitor.


* Created 31 May 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxOutputStream.h>
#include <ERxBufferOutputStream.h>
#include <ERxRedirectOutputStream.h>

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
//*************************************************//


//*********************Test case******************//

// static const prog_uchar prog_data[] PROGMEM = "0123456789" ; //length: 10
// prog_uchar is deprecated
static const char prog_data[] PROGMEM = "0123456789" ; //length: 10

void RunUnitTest()
{
	BegainUnitTest();
	
	// Test ERxBufferOutputStream
	{
		Serial.println("=>Test basic flow");

		unsigned char streamBuffer[64];
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		EqualTest("Initial size", outputStream.size(), 0);
		EqualTest("rdbuf", (int)outputStream.rdbuf(), (int)(&streamBuffer));

		size_t write_size = outputStream.print("ABC");
		EqualTest("write_size", write_size, 3);
		EqualTest("size", outputStream.size(), 3);
		EqualTest("data 0", streamBuffer[0], 'A');
		EqualTest("data 1", streamBuffer[1], 'B');
		EqualTest("data 2", streamBuffer[2], 'C');

		EqualTest("rdbuf() 0", outputStream.rdbuf()[0], 'A');

		outputStream.flush();
		EqualTest("Flush size", outputStream.size(), 0);
		EqualTest("Flush rdbuf", (int)outputStream.rdbuf(), (int)(&streamBuffer));
	}
	{
		Serial.println("=>Test overflow");

		unsigned char streamBuffer[2];
		ERxBufferOutputStream outputStream(streamBuffer, 2);

		size_t write_size = outputStream.print("ABC");
		EqualTest("write_size", write_size, 2);
		EqualTest("size", outputStream.size(), 2);
		EqualTest("data 0", streamBuffer[0], 'A');
		EqualTest("data 1", streamBuffer[1], 'B');
	}

	{
		Serial.println("=>Test read program memory");

		unsigned char streamBuffer[64];
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		size_t write_size = outputStream.printP(prog_data);
		EqualTest("write_size", write_size, 10);
		EqualTest("size", outputStream.size(), 10);
		EqualTest("data 0", streamBuffer[0], '0');
		EqualTest("data 9", streamBuffer[9], '9');
	}
	
	{
		Serial.println("=>Test read program memory");

		unsigned char streamBuffer[64];
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		size_t write_size = outputStream.writeP(prog_data, 5);
		EqualTest("write_size", write_size, 5);
		EqualTest("size", outputStream.size(), 5);
		EqualTest("data 0", streamBuffer[0], '0');
		EqualTest("data 4", streamBuffer[4], '4');
	}

	{
		Serial.println("=>Test print int type");

		unsigned char streamBuffer[64];
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		size_t write_size = outputStream.print(253);
		EqualTest("write_size", write_size, 3);
		EqualTest("size", outputStream.size(), 3);
		EqualTest("data 0", streamBuffer[0], '2');
		EqualTest("data 1", streamBuffer[1], '5');
		EqualTest("data 2", streamBuffer[2], '3');
	}

	{
		Serial.println("=>Test print hexadecimal type");

		unsigned char streamBuffer[64];
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		size_t write_size = outputStream.print(0xFA, HEX);
		EqualTest("write_size", write_size, 2);
		EqualTest("size", outputStream.size(), 2);
		EqualTest("data 0", streamBuffer[0], 'F');
		EqualTest("data 1", streamBuffer[1], 'A');
	}

	{
		Serial.println("=>Test print float type");

		unsigned char streamBuffer[64];
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		float data = 12.657896;
		size_t write_size = outputStream.print(data, 2);
		EqualTest("write_size", write_size, 5);
		EqualTest("size", outputStream.size(), 5);
		EqualTest("data 0", streamBuffer[0], '1');
		EqualTest("data 1", streamBuffer[1], '2');
		EqualTest("data 2", streamBuffer[2], '.');
		EqualTest("data 3", streamBuffer[3], '6');
		EqualTest("data 4", streamBuffer[4], '6');

		outputStream.print(' ');
		outputStream.print(data, 2);
		EqualTest("size", outputStream.size(), 11);
		EqualTest("data 6", streamBuffer[6], '1');
		EqualTest("data 7", streamBuffer[7], '2');
	}

	{
		Serial.println("=>Test write");

		unsigned char streamBuffer[64];
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		size_t write_size = outputStream.write('M');
		write_size = outputStream.write('N');
		EqualTest("write_size", write_size, 1);
		EqualTest("data 0", streamBuffer[0], 'M');
		EqualTest("data 1", streamBuffer[1], 'N');
	}

	// Test ERxRedirectOutputStream
	{
		Serial.println("=>Test basic flow");

		unsigned char streamBuffer[64] = {0};
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		unsigned char redirectBuffer[64];
		ERxRedirectOutputStream redirectStream(&outputStream, redirectBuffer, 64);
		{
			const char * pPrefix = "0123 "; // length 5
			redirectStream.SetPrefix((uint8_t*)pPrefix, 5);
		}

		size_t write_size = redirectStream.print("abc\r\n"); // length 5. total: 10
		EqualTest("write_size", write_size, 5);
		EqualTest("size", outputStream.size(), 10);
		EqualTest("data 0", streamBuffer[0], '0');
		EqualTest("data 1", streamBuffer[1], '1');
		EqualTest("data 5", streamBuffer[5], 'a');
		EqualTest("data 8", streamBuffer[8], '\r');
		EqualTest("data 9", streamBuffer[9], '\n');

		write_size = redirectStream.print("ABC\r\n"); // length 5 . total: 20
		EqualTest("write_size", write_size, 5);
		EqualTest("size", outputStream.size(), 20);
		EqualTest("data 0", streamBuffer[0], '0');
		EqualTest("data 5", streamBuffer[5], 'a');
		EqualTest("data 10", streamBuffer[10], '0');
		EqualTest("data 15", streamBuffer[15], 'A');
		EqualTest("data 19", streamBuffer[19], '\n');

		redirectStream.SetPrefix(NULL, 0); // Clear prefix

		write_size = redirectStream.print("ABC\r\n"); // length 5 . total: 25
		EqualTest("write_size", write_size, 5);
		EqualTest("size", outputStream.size(), 25);
		EqualTest("data 20", streamBuffer[20], 'A');

		{
			const char * pPrefix = "MNOP "; // length 5
			redirectStream.SetPrefix((uint8_t*)pPrefix, 5);
		}
		
		write_size = redirectStream.print("123\r\n"); // length 5. total: 35
		EqualTest("write_size", write_size, 5);
		EqualTest("size", outputStream.size(), 35);
		EqualTest("data 25", streamBuffer[25], 'M');
		EqualTest("data 30", streamBuffer[30], '1');
		EqualTest("data 34", streamBuffer[34], '\n');
	}

	{
		Serial.println("=>Test overflow");

		unsigned char streamBuffer[8];
		ERxBufferOutputStream outputStream(streamBuffer, 8);

		unsigned char redirectBuffer[64];
		ERxRedirectOutputStream redirectStream(&outputStream, redirectBuffer, 64);
		{
			const char * pPrefix = "0123 "; // length 5
			redirectStream.SetPrefix((uint8_t*)pPrefix, 5);
		}

		size_t write_size = redirectStream.print("abc\r\n"); // length 5.
		EqualTest("write_size", write_size, 5);
		EqualTest("size", outputStream.size(), 8);
		EqualTest("data 0", streamBuffer[0], '0');
		EqualTest("data 1", streamBuffer[1], '1');
		EqualTest("data 5", streamBuffer[5], 'a');
		EqualTest("data 7", streamBuffer[7], 'c');
	}

	{
		Serial.println("=>Test cache");

		unsigned char streamBuffer[64] = {0};
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		unsigned char redirectBuffer[64] = {0};
		ERxRedirectOutputStream redirectStream(&outputStream, redirectBuffer, 64);
		{
			const char * pPrefix = "0123 "; // length 5
			redirectStream.SetPrefix((uint8_t*)pPrefix, 5);
		}

		size_t write_size = redirectStream.print("abc"); // length 3
		EqualTest("write_size", write_size, 3);
		EqualTest("size", outputStream.size(), 0);
		EqualTest("cache 0", redirectBuffer[0], '0');
		EqualTest("cache 1", redirectBuffer[1], '1');
		EqualTest("cache 5", redirectBuffer[5], 'a');
		EqualTest("cache 7", redirectBuffer[7], 'c');
	}

	{
		Serial.println("=>Test cache overflow");

		unsigned char streamBuffer[64] = {0};
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		unsigned char redirectBuffer[8] = {0};
		ERxRedirectOutputStream redirectStream(&outputStream, redirectBuffer, 8);
		{
			const char * pPrefix = "0123 "; // length 5
			redirectStream.SetPrefix((uint8_t*)pPrefix, 5);
		}

		size_t write_size = redirectStream.print("abc\r\n"); // length 5
		EqualTest("write_size", write_size, 5);
		EqualTest("size", outputStream.size(), 15);
		EqualTest("data 0", streamBuffer[0], '0');
		EqualTest("data 5", streamBuffer[5], 'a');
		EqualTest("data 7", streamBuffer[7], 'c');
		EqualTest("data 8", streamBuffer[8], '0');
		EqualTest("data 14", streamBuffer[14], '\n');
	}

	{
		Serial.println("=>Test CR LF");

		unsigned char streamBuffer[64] = {0};
		ERxBufferOutputStream outputStream(streamBuffer, 64);

		unsigned char redirectBuffer[64] = {0};
		ERxRedirectOutputStream redirectStream(&outputStream, redirectBuffer, 64);
		{
			const char * pPrefix = "0123 "; // length 5
			redirectStream.SetPrefix((uint8_t*)pPrefix, 5);
		}

		size_t write_size = redirectStream.print("\r\n"); // length 2
		EqualTest("write_size", write_size, 2);
		EqualTest("size", outputStream.size(), 7);
		EqualTest("data 0", streamBuffer[0], '0');
		EqualTest("data 5", streamBuffer[5], '\r');
		EqualTest("data 6", streamBuffer[6], '\n');
	}

	EndUnitTest();
}