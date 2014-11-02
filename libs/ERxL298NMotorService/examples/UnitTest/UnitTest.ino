/*
ERxL298NMotorService unit test

This sketch is used to test if the addXxxMotor logic works correctly. 

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Verfiy the result in the PC Serial monitor.


* Created 29 May 2012
* By Jeffrey Sun
* 

*/

#include <Arduino.h>
#include <HardwareSerial.h>
#include <ERxOutputStream.h>
#include <ERxIOStream.h>
#include <ERxService.h>
#include <ERxServiceContext.h>
#include <ERxIMotor.h>
#include <ERxL298N.h>
#include <ERxL298NMotorService.h>

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
void Test();

void RunUnitTest()
{
	BegainUnitTest();

	Test();
	
	EndUnitTest();
}

void Test()
{
	{
		Serial.println("Test addLeftMotor/addRightMotor");
		ERxL298NMotorService motorService;
		bool ret = motorService.addLeftMotor(4,5);
		EqualTest("addLeftMotor #0: ", ret, true);
		ret = motorService.addLeftMotor(4,5);
		EqualTest("addLeftMotor #1: ", ret, true);
		ret = motorService.addLeftMotor(4,5);
		EqualTest("addLeftMotor #2: ", ret, false);

		ret = motorService.addRightMotor(4,5);
		EqualTest("addRightMotor #0: ", ret, true);
		ret = motorService.addRightMotor(4,5);
		EqualTest("addRightMotor #1: ", ret, true);
		ret = motorService.addLeftMotor(4,5);
		EqualTest("addRightMotor #2: ", ret, false);
	}
}
