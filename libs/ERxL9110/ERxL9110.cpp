#include "ERxL9110.h"

ERxL9110::ERxL9110(unsigned char a, unsigned char b, bool reverse /*= false*/)
	:m_a(a), m_b(b), m_reverse(reverse)
{
	pinMode(m_a, OUTPUT);
	pinMode(m_b, OUTPUT);
	stop();
}

void ERxL9110::forward(unsigned int speed /*= 0*/)
{
	digitalWrite(m_a, m_reverse?HIGH:LOW);
	digitalWrite(m_b, m_reverse?LOW:HIGH);
}

void ERxL9110::backward(unsigned int speed /*= 0*/)
{
	digitalWrite(m_a, m_reverse?LOW:HIGH);
	digitalWrite(m_b, m_reverse?HIGH:LOW);
}

void ERxL9110::stop()
{
	digitalWrite(m_a, LOW);
	digitalWrite(m_b, LOW);
}