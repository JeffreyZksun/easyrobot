/*
ERxCommandExecutorService unit test

This sketch is used to test ERxCommandExecutorService 

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Verfiy the result in the PC Serial monitor.


* Created 3 June 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxCommandExecutorService.h>
#include <ERxOutputStream.h>
#include <ERxIOStream.h>
#include <ERxService.h>
#include <ERxServiceContext.h>

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

		ERxCommandExecutorService cmdExeService(12, "result\r\n"); // result length: 8

		uint8_t sCmdBuffer[64];
		uint8_t sResultBuffer[64];
		ERxServiceContext context(sCmdBuffer, 64, sResultBuffer, 64);

		context.InvalidateCommand();
		cmdExeService.Populate(context);
		EqualTest("Populate", context.HasValidCommand(), false);

		context.SetCommandId(12);
		context.GetCommandParameterStream().print("254\r\n");

		context.BeginCommandExecution();
		cmdExeService.Execute(context);
		context.EndCommandExecution();

		EqualTest("Execute", context.HasValidCommand(), true);
		EqualTest("IsCommandExecuted", context.IsCommandExecuted(), true);
		EqualTest("IsCommandSuccess", context.IsCommandSuccess(), true);
		EqualTest("Result size", (int)context.GetResultStream().size(), 8);
		EqualTest("Result 0", sResultBuffer[0], 'r');
		EqualTest("Result 1", sResultBuffer[1], 'e');
		EqualTest("Result 2", sResultBuffer[2], 's');
		EqualTest("Result 3", sResultBuffer[3], 'u');
		EqualTest("Result 4", sResultBuffer[4], 'l');
		EqualTest("Result 5", sResultBuffer[5], 't');
		EqualTest("Result 6", sResultBuffer[6], '\r');
		EqualTest("Result 7", sResultBuffer[7], '\n');
	}

	{
		Serial.println("=>Test other command");

		ERxCommandExecutorService cmdExeService(12, "result\r\n"); // result length: 8

		uint8_t sCmdBuffer[64] = {0};
		uint8_t sResultBuffer[64] = {0};
		ERxServiceContext context(sCmdBuffer, 64, sResultBuffer, 64);

		context.SetCommandId(37);
		context.GetCommandParameterStream().print("254\r\n");

		context.BeginCommandExecution();
		cmdExeService.Execute(context);
		context.EndCommandExecution();

		EqualTest("Execute", context.HasValidCommand(), true);
		EqualTest("IsCommandExecuted", context.IsCommandExecuted(), false);
		EqualTest("Result size", (int)context.GetResultStream().size(), 0);
	}

	
	EndUnitTest();
}