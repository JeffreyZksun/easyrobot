
#include <ERxTextMessage.h>


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

void EqualTest(const char* pCaseName, int actualValue, int expectedValue)
{
	tatalCases++;
	
	OutPutCompareMessage(pCaseName, actualValue == expectedValue);
}

void EqualTest(const char* pCaseName, unsigned int actualValue, int expectedValue)
{
	EqualTest(pCaseName, (int) actualValue, (int) expectedValue);
}

void EqualTest(const char* pCaseName, int actualValue, unsigned int expectedValue)
{
	EqualTest(pCaseName, (int) actualValue, (int) expectedValue);
}


void EqualTest(const char* pCaseName, unsigned int actualValue, unsigned int expectedValue)
{
	tatalCases++;
	
	OutPutCompareMessage(pCaseName, actualValue == expectedValue);
}

void EqualTest(const char* pCaseName, long actualValue, long expectedValue)
{
	tatalCases++;
	
	OutPutCompareMessage(pCaseName, actualValue == expectedValue);
}

void EqualTest(const char* pCaseName, unsigned long actualValue, unsigned long expectedValue)
{
	tatalCases++;
	
	OutPutCompareMessage(pCaseName, actualValue == expectedValue);
}

//void EqualTest(const char* pCaseName, bool actualValue, bool expectedValue)
//{
//	tatalCases++;
//	
//	OutPutCompareMessage(pCaseName, actualValue == expectedValue);
//}

void EqualTest(const char* pCaseName, float actualValue, float expectedValue)
{
	tatalCases++;

	//Serial.print("float-->");
	double diff = actualValue - expectedValue;
	OutPutCompareMessage(pCaseName, diff > -1e-5 && diff < 1e-5); // The tolerance should be not less than 1e-5.
}

void EqualTest(const char* pCaseName, double actualValue, double expectedValue)
{
	tatalCases++;

	//Serial.print("double-->");
	double diff = actualValue - expectedValue;
	OutPutCompareMessage(pCaseName, diff > -1e-5 && diff < 1e-5);
}

void RunUnitTest()
{
	BegainUnitTest();

	/**************************************/
	/*   Test  ERxIORingBufferStream      */
	/**************************************/
	Serial.println("***Test  ERxIORingBufferStream***");
	{
		ERxIORingBufferStream ioStream;

		// Test intial satus
		EqualTest("ctor available()", ioStream.available(), 0);
		EqualTest("ctor full()", ioStream.full(), false);
		EqualTest("ctor capacity()", ioStream.capacity(), BUFFER_CAPACITY - 1);
		EqualTest("ctor peekFind()", ioStream.peekFind("c", 1), -1);

		// Test print
		ioStream.print("0123456789");
		EqualTest("print available()", ioStream.available(), 10);
		EqualTest("print full()", ioStream.full(), false);
		EqualTest("print capacity()", ioStream.capacity(), BUFFER_CAPACITY - 1);

		// Test peekFind
		EqualTest("print peekFind()", ioStream.peekFind("0", 1), 0);
		EqualTest("print peekFind()", ioStream.peekFind("123", 3), 1);
		EqualTest("print peekFind()", ioStream.peekFind("9", 1), 9);

		// Test flush
		ioStream.flush();
		EqualTest("flush available()", ioStream.available(), 0);
		EqualTest("flush full()", ioStream.full(), false);
		EqualTest("flush capacity()", ioStream.capacity(), BUFFER_CAPACITY - 1);
		EqualTest("flush peekFind()", ioStream.peekFind("c", 1), -1);
	}

	// Test number converter
	{
		ERxIORingBufferStream ioStream;
		long expected = 23;
		ioStream.print(expected);
		EqualTest("print int", ioStream.available(), 2);

		long actual = ioStream.parseInt();
		EqualTest("parse int", ioStream.available(), 0);
		EqualTest("parse int", actual, expected);
	}

	{
		ERxIORingBufferStream ioStream;
		float expected = 24.56;
		ioStream.print(expected);

		float actual = ioStream.parseFloat();
		EqualTest("parse float", ioStream.available(), 0);
		EqualTest("parse float", actual, expected);
	}

	{
		ERxIORingBufferStream ioStream;
		unsigned long expected = 16847;
		ioStream.print(expected);

		unsigned long actual = ioStream.parseULong();
		EqualTest("parse ulong", ioStream.available(), 0);
		EqualTest("parse ulong", actual, expected);
	}

	{
		ERxIORingBufferStream ioStream;
		unsigned long expected = 16847;
		ioStream.print("leadingdummy");
		ioStream.print(expected);
		ioStream.print("one");

		unsigned long actual = ioStream.parseULong();
		EqualTest("parse ulong", ioStream.available(), 3);
		//Serial.print(ioStream.available());
		//while(ioStream.available())
		//	Serial.write(ioStream.read());
		//Serial.println();
		EqualTest("parse ulong", actual, expected);
	}

	{
		ERxIORingBufferStream ioStream;
		unsigned long expected = 0xFF48;
		ioStream.print("0xFF48");

		unsigned long actual = ioStream.parseULong();
		EqualTest("parse ulong", ioStream.available(), 0);
		EqualTest("parse ulong", actual, expected);
	}

		{
		ERxIORingBufferStream ioStream;
		unsigned long expected = 0xFF48;
		ioStream.print("0Xff48");

		unsigned long actual = ioStream.parseULong();
		EqualTest("parse ulong", ioStream.available(), 0);
		EqualTest("parse ulong", actual, expected);
	}

	{
		ERxIORingBufferStream ioStream;
		unsigned long expected = 0xFF48;
		ioStream.print("leadingdummy");
		ioStream.print("0xFF48");
		ioStream.print("one");

		unsigned long actual = ioStream.parseULong();
		EqualTest("parse ulong", ioStream.available(), 3);
		EqualTest("parse ulong", actual, expected);
	}

	{
		ERxIORingBufferStream ioStream;
		ioStream.print("0123456789\r\ncdef");
		int dist = ioStream.peekFind("\r\n", 2);
		EqualTest("peekFind <CR><LF>", dist, 10);

		ERxIORingBufferStream textMsg;
		unsigned int len = ioStream.writeTo(&textMsg, dist + 2);
		EqualTest("writeTo", len, dist + 2);
		EqualTest("writeTo available()", ioStream.available(), 4);
		EqualTest("writeTo available()", textMsg.available(), 12);
	}
	
	{
		ERxIORingBufferStream ioStream;
		ioStream.print("0123\r\n6789\r\n.\r\n0123");
		int dist = ioStream.peekFind("\r\n.\r\n", 5);
		EqualTest("peekFind <CR><LF>.CR><LF>", dist, 10);

		ERxIORingBufferStream textMsg;
		unsigned int len = ioStream.writeTo(&textMsg, dist + 5);
		EqualTest("writeTo", len, dist + 5);
		EqualTest("writeTo available()", ioStream.available(), 4);
		EqualTest("writeTo available()", textMsg.available(), 15);
	}

	{
		ERxIORingBufferStream ioStream;
		ioStream.print("253");
		int expected = ioStream.read();
		int actual = ioStream.putback();
		EqualTest("putback ", actual, expected);
		EqualTest("putback available()", ioStream.available(), 3);
	}

	// ToDo - add test case for overflow

	/**************************************/
	/*   Test  ERxTextMessage             */
	/**************************************/

	Serial.println("***Test  ERxTextMessage***");

	// Test read message from iostream
	{
		ERxIORingBufferStream ioStream;
		ERxTextMessage textMsg(&ioStream);
		EqualTest("ctor available()", textMsg.available(), 0);

		ioStream.print("Robot 25.4 73\r\n15");
		EqualTest("parse available()", textMsg.available() != 0, true);
		EqualTest("parse available()", ioStream.available(), 0);

		char buffer[16];
		unsigned int len = textMsg.readBytesUntil(' ', buffer, 16);
		EqualTest("read readBytesUntil", len, 5);

		float actualFloat = textMsg.parseFloat();
		EqualTest("read float", actualFloat, (float)25.4);

		int actualInt = textMsg.parseInt();
		EqualTest("read int", actualInt, 73);

		textMsg.invalidate();
		EqualTest("invalidate available()", textMsg.available(), 0);

		ioStream.flush();
		ioStream.print("1\r\ndummy");
		EqualTest("parse available()", textMsg.available() != 0, true);
		actualInt = textMsg.parseInt();
		EqualTest("read int", actualInt, 151);
	}

	// Test write message to iostream
	{
		ERxIORingBufferStream ioStream;
		ERxTextMessage textMsg(&ioStream);
		
		textMsg.print("Robot 25.4 73\r\n\15");
		EqualTest("parse available()", textMsg.available() != 0, true);

		char buffer[16];
		unsigned int len = textMsg.readBytesUntil(' ', buffer, 16);
		EqualTest("read readBytesUntil", len, 5);

		float actualFloat = textMsg.parseFloat();
		EqualTest("read float", actualFloat, (float)25.4);

		int actualInt = textMsg.parseInt();
		EqualTest("read int", actualInt, 73);

		textMsg.invalidate();
		EqualTest("invalidate available()", textMsg.available(), 0);
	}
		
	// Test keyword ECHO
	{
		ERxIORingBufferStream ioStream;
		ERxTextMessage textMsg(&ioStream);
		
		textMsg.print("ECHO\r\n");
		EqualTest("echo available()", textMsg.available() != 0, false);
		EqualTest("echo available()", ioStream.available(), 2);
	}

	// Test keyword ECHO
	{
		ERxIORingBufferStream ioStream;
		ERxTextMessage textMsg(&ioStream);
		
		textMsg.print("ECHO int 123\r\n");
		EqualTest("echo available()", textMsg.available() != 0, false);
		EqualTest("echo available()", ioStream.available(), 10);

		char buffer[16];
		ioStream.read(); // Discard the leading space
		unsigned int len = ioStream.readBytesUntil(' ', buffer, 16);
		EqualTest("read readBytesUntil", len, 3);

		int actualInt = ioStream.parseInt();
		EqualTest("read int", actualInt, 123);

	}

	// Test keyword DATA
	{
		ERxIORingBufferStream ioStream;
		ERxTextMessage textMsg(&ioStream);
		
		textMsg.print("DATA\r\n"); //6
		EqualTest("data available()", textMsg.available(), 0);
		EqualTest("data available()", ioStream.available(), 0);

		textMsg.print("012\r\n01234567\r\n012\r\n01234567\r\n.\r\ndummy"); //33
		EqualTest("data available()", textMsg.available(), 33 + 6);
	}

	// Test keyword DATA
	{
		ERxIORingBufferStream ioStream;
		ERxTextMessage textMsg(&ioStream);
		
		textMsg.print("DATA dummy\r\n"); //12
		EqualTest("data available()", textMsg.available(), 0);
		EqualTest("data available()", ioStream.available(), 0);

		textMsg.print("012\r\n01234567\r\n012\r\n01234567\r\n.\r\ndummy"); //33
		EqualTest("data available()", textMsg.available(), 33 + 12);
	}

	// Test keyword DATA sepcial case
	{
		ERxIORingBufferStream ioStream;
		ERxTextMessage textMsg(&ioStream);
		
		ioStream.print("DATA\r\nline\r\n\r\n.\r\n"); 
		textMsg.available();
		EqualTest("special data available()", textMsg.available() != 0, true);
	}

	// ToDo - add test case for overflow

	EndUnitTest();
}

void setup() {

	Serial.begin(9600);

	RunUnitTest();

}

void loop() {

	
}

