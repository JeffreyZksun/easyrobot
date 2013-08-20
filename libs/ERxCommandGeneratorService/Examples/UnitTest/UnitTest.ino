/*
ERxCommandGeneratorService unit test

This sketch is used to test ERxCommandGeneratorService 

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Verfiy the result in the PC Serial monitor.


* Created 3 June 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxCommandGeneratorService.h>
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

		ERxCommandGeneratorService cmdGenService(12, "254\r\n", NULL);

		uint8_t sCmdBuffer[64];
		uint8_t sResultBuffer[64];
		ERxServiceContext context(sCmdBuffer, 64, sResultBuffer, 64);

		context.InvalidateCommand();
		cmdGenService.Populate(context);

		EqualTest("Populate", context.HasValidCommand(), true);

		context.BeginCommandExecution();
		EqualTest("cmd id", (int)context.GetCommandId(), 12);
		EqualTest("parameter size", (int)context.GetCommandParameterStream().available(), 5);
		EqualTest("para 0", (int)context.GetCommandParameterStream().read(), '2');
		EqualTest("para 1", (int)context.GetCommandParameterStream().read(), '5');
		EqualTest("para 2", (int)context.GetCommandParameterStream().read(), '4');
		EqualTest("para 3", (int)context.GetCommandParameterStream().read(), '\r');
		EqualTest("para 4", (int)context.GetCommandParameterStream().read(), '\n');
		context.EndCommandExecution();

		context.BeginCommandExecution();
		cmdGenService.Execute(context);
		context.EndCommandExecution();

		EqualTest("Execute", context.HasValidCommand(), true);
		EqualTest("IsCommandExecuted", context.IsCommandExecuted(), false);
		EqualTest("Result", (int)context.GetResultStream().size(), 0);
	}
	
	EndUnitTest();
}