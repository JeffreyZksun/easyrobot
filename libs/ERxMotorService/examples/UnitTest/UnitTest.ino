/*
ERxMotorService unit test

This sketch is used to test if the addXxxMotor logic works correctly. 

Steps to run this sketch
 1. Build the sketch and upload to Arduino
 2. Verfiy the result in the PC Serial monitor.


Note:
Change MAX_MOTORS to be 2 before running the unit test. 
#define MAX_MOTORS 2

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
#include <ERxMotorService.h>

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
		// make sure #define MAX_MOTORS 2.
		Serial.println("Test addLeftMotor/addRightMotor");
		ERxMotorService motorService;

		ERxL298N m1(4,5);
		bool ret = motorService.addLeftMotor(&m1);
		EqualTest("addLeftMotor #0: ", ret, true);

		ERxL298N m2(6,7);
		ret = motorService.addLeftMotor(&m2);
		EqualTest("addLeftMotor #1: ", ret, true);
		
		ERxL298N m3(8,9);
		ret = motorService.addLeftMotor(&m3);
		EqualTest("addLeftMotor #2: ", ret, false);

		ERxL298N m4(10,11);
		ret = motorService.addRightMotor(&m4);
		EqualTest("addRightMotor #0: ", ret, true);

		ERxL298N m5(12,13);
		ret = motorService.addRightMotor(&m5);
		EqualTest("addRightMotor #1: ", ret, true);

		ERxL298N m6(14,15);
		ret = motorService.addRightMotor(&m6);
		EqualTest("addRightMotor #2: ", ret, false);
	}
}
