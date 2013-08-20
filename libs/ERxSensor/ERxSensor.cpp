#include <ERxSensor.h>
#include <wiring.h>

ERxSensor::ERxSensor():m_Index(255), m_bIsDigital(true), m_bIsInput(true),m_bIsHigh(false)
{
	
}

ERUINT32 ERxSensor::GetValue()
{
	if(!m_bIsInput) // read output
	{
		return ReadDigitalOutputValue();
	}
	else // input
	{
		if(m_bIsDigital)
			return ReadDigitalInputValue();
		else
			return ReadAnalogInputValue();
	}
}

bool ERxSensor::SetData(ERUINT8 index, bool bIsInput, bool bIsDigital, bool bIsHigh)
{
	bool bNeedUpdatePin = false;

	if(m_Index != index)
	{
		m_Index = index;
		bNeedUpdatePin = true;
	}

	if(m_bIsInput != bIsInput)
	{
		m_bIsInput = bIsInput;
		bNeedUpdatePin = true;
	}
	
	m_bIsDigital = bIsDigital;

	if(m_bIsHigh != bIsHigh)
	{
		m_bIsHigh = bIsHigh;
		UpdatePinOutput();
	}

	if(bNeedUpdatePin)
	{
		UpdatePinMode();
		UpdatePinOutput();
	}
}

ERUINT32 ERxSensor::ReadAnalogInputValue()
{
	// read value
	return analogRead(m_Index); // Note: the used index for analog pin is different from digital pin
}

ERUINT32 ERxSensor::ReadDigitalInputValue()
{
	ERUINT8 digitalPinIndexInArdunioBoard = GetArdunioMegaPinIndex();

	// read value
	// ToDo we reuse the analog pin here, so don't use digitalRead.
	return digitalRead(digitalPinIndexInArdunioBoard);
}

ERUINT32 ERxSensor::ReadDigitalOutputValue()
{
	ERUINT8 digitalPinIndexInArdunioBoard = GetArdunioMegaPinIndex();

	// read value
	// ToDo we reuse the analog pin here, so don't use digitalRead.
	return digitalRead(digitalPinIndexInArdunioBoard);
}

ERUINT8  ERxSensor::GetArdunioMegaPinIndex()
{
	// Based on the pin definition in pins_arduino.c
	// The index of analog pin begins with 54
	ERUINT8 digitalPinIndexInArdunioBoard = m_Index + 54;

	return digitalPinIndexInArdunioBoard;
}

void ERxSensor::UpdatePinMode()
{
	ERUINT8 digitalPinIndexInArdunioBoard = GetArdunioMegaPinIndex();

	pinMode(digitalPinIndexInArdunioBoard, m_bIsInput ? INPUT : OUTPUT);
}

void ERxSensor::UpdatePinOutput()
{
	if(!m_bIsInput)
	{
		ERUINT8 digitalPinIndexInArdunioBoard = GetArdunioMegaPinIndex();
		digitalWrite(digitalPinIndexInArdunioBoard, m_bIsHigh ? HIGH : LOW);
	}
}
