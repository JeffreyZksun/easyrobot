/*
ERxService unit test

This sketch is used to test if the data process logic works correctly. 

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Verfiy the result in the PC Serial monitor.


* Created 29 May 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxHost.h>
#include <ERxService.h>
#include <ERxIOStream.h>
#include <ERxOutputStream.h>
#include <ERxCommandExecutorService.h>
#include <ERxCommandGeneratorService.h>

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
void TestERxServiceContext();
void TestFramework();

void RunUnitTest()
{
	BegainUnitTest();

	TestERxServiceContext();
	TestFramework();
	
	EndUnitTest();
}

void TestERxServiceContext()
{
	{
		Serial.println("Test work flow");
		uint8_t cmdParaBuffer[32];
		uint8_t resultBuffer[32];

		ERxServiceContext context(cmdParaBuffer, 32, resultBuffer, 32);

		EqualTest("HasValidCommand: ", context.HasValidCommand(), false);
		EqualTest("IsCommandExecuted: ", context.IsCommandExecuted(), false);
		EqualTest("IsCommandSuccess: ", context.IsCommandSuccess(), false);

		EqualTest("parameter buffer size: ", context.GetCommandParameterStream().available(), 0);
		EqualTest("result buffer size: ", context.GetResultStream().size(), 0);

		context.SetCommandId(12);
		EqualTest("commd id: ", (int)context.GetCommandId(), 12);
		context.SetCommandId(53);
		EqualTest("commd id: ", (int)context.GetCommandId(), 53);

		EqualTest("HasValidCommand: ", context.HasValidCommand(), true);
		EqualTest("IsCommandExecuted: ", context.IsCommandExecuted(), false);
		EqualTest("IsCommandSuccess: ", context.IsCommandSuccess(), false);

		ERxIOStream& rParaStream = context.GetCommandParameterStream();
		rParaStream.print("abc\r\n"); // length: 5.
		EqualTest("parameter length: ", context.GetCommandParameterStream().available(), 5);

		context.SetIsCommandExecuted(true);
		context.SetIsCommandSuccess(true);
		EqualTest("IsCommandExecuted: ", context.IsCommandExecuted(), true);
		EqualTest("IsCommandSuccess: ", context.IsCommandSuccess(), true);

		context.InvalidateCommand();

		EqualTest("HasValidCommand: ", context.HasValidCommand(), false);
	}

	{
		Serial.println("Command execution");
		uint8_t cmdParaBuffer[32];
		uint8_t resultBuffer[32];

		ERxServiceContext context(cmdParaBuffer, 32, resultBuffer, 32);
		context.SetCommandId(12);
		context.GetCommandParameterStream().print("132\r\n"); // length: 5.

		context.BeginCommandExecution();
		{
			EqualTest("result buffer size: ", context.GetResultStream().size(), 0);
			EqualTest("HasValidCommand: ", context.HasValidCommand(), true);
			EqualTest("IsCommandExecuted: ", context.IsCommandExecuted(), false);

			const unsigned long data = context.GetCommandParameterStream().parseULong();
			EqualTest("Parse number: ", data, 132);
			EqualTest("Paramenter size: ", context.GetCommandParameterStream().available(), 2);

			context.GetResultStream().print("0123456789");// length: 10
			EqualTest("result buffer size: ", context.GetResultStream().size(), 10);

			context.SetIsCommandExecuted(true);
			context.SetIsCommandSuccess(true);
			EqualTest("IsCommandExecuted: ", context.IsCommandExecuted(), true);
			EqualTest("IsCommandSuccess: ", context.IsCommandSuccess(), true);
		}
		context.EndCommandExecution();

		EqualTest("parameter buffer size: ", context.GetCommandParameterStream().available(), 5);
		EqualTest("result buffer size: ", context.GetResultStream().size(), 10);

		EqualTest("IsCommandExecuted: ", context.IsCommandExecuted(), true);
		EqualTest("IsCommandSuccess: ", context.IsCommandSuccess(), true);

		context.BeginCommandExecution();
		{
			EqualTest("result buffer size: ", context.GetResultStream().size(), 0);
			EqualTest("HasValidCommand: ", context.HasValidCommand(), true);
			EqualTest("IsCommandExecuted: ", context.IsCommandExecuted(), false);

			const unsigned long data = context.GetCommandParameterStream().parseULong();
			EqualTest("Parse number: ", data, 132);
			EqualTest("Paramenter size: ", context.GetCommandParameterStream().available(), 2);

			context.GetResultStream().print("0123456789");// length: 10
			EqualTest("result buffer size: ", context.GetResultStream().size(), 10);

			context.SetIsCommandExecuted(true);
			context.SetIsCommandSuccess(true);
			EqualTest("IsCommandExecuted: ", context.IsCommandExecuted(), true);
			EqualTest("IsCommandSuccess: ", context.IsCommandSuccess(), true);
		}
		context.EndCommandExecution();		
	}

	{
		Serial.println("Command execution");
		uint8_t cmdParaBuffer[32];
		uint8_t resultBuffer[32];

		ERxServiceContext context(cmdParaBuffer, 32, resultBuffer, 32);
		EqualTest("My address: ", (int)context.GetMyAddress(), 255);

		context.SetMyAddress(10);
		EqualTest("My address: ", (int)context.GetMyAddress(), 10);

		context.SetMyAddress(150);
		EqualTest("My address: ", (int)context.GetMyAddress(), 150);
	}
}

void TestFramework()
{
	static uint8_t sCmdBuffer[64];
	static uint8_t sResultBuffer[64];

	char cmdData[3]="12";

	ERxHost host(sCmdBuffer, 64, sResultBuffer, 64);
	ERxCommandExecutorService exeService(200, cmdData);
	ERxCommandGeneratorService cmdGenService(200, cmdData, &Serial);

	host.AddService(&cmdGenService);
	host.AddService(&exeService);

	host.Run();

	// Verify the command parameter is correct.

	{
		EqualTest("CommandPara 0: ", sCmdBuffer[0], cmdData[0]);
		EqualTest("CommandPara 1: ", sCmdBuffer[1], cmdData[1]);
	}

	// Verify the output result is correct.

	{
		EqualTest("Result 0: ", sResultBuffer[0], cmdData[0]);
		EqualTest("Result 1: ", sResultBuffer[1], cmdData[1]);
	}
}