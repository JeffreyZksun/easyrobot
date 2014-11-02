#include "ERxL298N.h"

ERxL298N::ERxL298N(unsigned char e, unsigned char m, bool reverse /*= false*/)
	:m_e(e), m_m(m), m_reverse(reverse)
{
	pinMode(m_e, OUTPUT);
	pinMode(m_m, OUTPUT);
	stop();
}

void ERxL298N::forward(unsigned int speed /*= 0*/)
{
	digitalWrite(m_e, HIGH);
	digitalWrite(m_m, m_reverse?LOW:HIGH);
}

void ERxL298N::backward(unsigned int speed /*= 0*/)
{
	digitalWrite(m_e, HIGH);
	digitalWrite(m_m, m_reverse?HIGH:LOW);
}

void ERxL298N::stop()
{
	digitalWrite(m_e, LOW);
}