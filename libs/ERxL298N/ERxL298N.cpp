#include "ERxL298N.h"

ERxL298N::ERxL298N(unsigned char e, unsigned char m)
	:m_e(e), m_m(m)
{
	pinMode(m_e, OUTPUT);
	pinMode(m_m, OUTPUT);
	stop();
}

void ERxL298N::forward()
{
	digitalWrite(m_e, HIGH);
	digitalWrite(m_m, HIGH);
}

void ERxL298N::backward()
{
	digitalWrite(m_e, HIGH);
	digitalWrite(m_m, LOW);
}

void ERxL298N::stop()
{
	digitalWrite(m_e, LOW);
}