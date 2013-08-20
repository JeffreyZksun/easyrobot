#pragma once
#ifndef GRAPHICROBOT_H
#define GRAPHICROBOT_H

void SetPWMMotor(int index, int speed);

bool TestSensor(int index, bool bIsOn, int minValue, int maxValue, bool bIsDigitalSensor);

void Delay(int microSeconds);

void TextOutput(char* outputMessage);

#endif // GRAPHICROBOT_H