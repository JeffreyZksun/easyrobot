#pragma once
#ifndef ROBOTSENSOR_H
#define ROBOTSENSOR_H

#include <ERxType.h>

class ERxSensor
{
public:
	ERxSensor();

	ERUINT32 GetValue();

	bool	SetData(ERUINT8 index, bool bIsInput, bool bIsDigital, bool bIsHigh);

private:
	ERUINT32 ReadAnalogInputValue();
	ERUINT32 ReadDigitalInputValue();
	ERUINT32 ReadDigitalOutputValue();

	ERUINT8  GetArdunioMegaPinIndex();

	void	UpdatePinMode();
	void	UpdatePinOutput();

private:
	ERUINT8		m_Index;

	// ToDo - we can use a bit to save a bool value.
	bool		m_bIsDigital;
	bool		m_bIsInput;
	bool		m_bIsHigh;
};
#endif // ROBOTSENSOR_H