/*
ERxL9110.h - library for Arduino to control L9110 DC motor driver. 
Copyright (c) 2011 Jeffrey Sun.  All right reserved.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
Function
* This library is used to control L9110 DC motor driver.
* It only controls to rotation direction. The rotation speed is not controlled.

I/O table of L9110.

IA1/IA2 	IB1/IB2			Motor
1 				0		rotate forward
0 				1		rotate backward
1 				1			stop
0 				0			stop


The circuit:
---------------------------------------------
	Arduino		L9110(Motor Driver)			
	4*				IA1
	5*				IB1
	6*				IA2
	7*				IB2

	GND				GND
	

The pin number with the * suffix means you can change it to be any number.


---------------------------------------------
	Motor Battery	   L9110	 Ardiono
	GND					GND		  GND
	+2.5~12V			VCC		  N/A

---------------------------------------------
	Logic Battery	   	 Ardiono
	GND					  GND
	+7~12V			  	  Vin

---------------------------------------------
	L9110		Motor
	MotorA			DC Motor
	MotorB			DC Motor
---------------------------------------------

L9110 reference: http://detail.tmall.com/item.htm?id=38705079159&ali_refid=a3_420434_1006:1107554683:N:l9110:63aa3b7bdc4ba6cb9a71a41810705d57&ali_trackid=1_63aa3b7bdc4ba6cb9a71a41810705d57&spm=a230r.1.0.0.T0s7wQ

一 模块描述
1 双L9110S芯片的电机驱动
2 模块供电电压：2.5-12V
3 适合的电机范围：电机工作电压2.5v-12V之间，最大工作电流0.8A，目前市面上的智能小车电压和电流都在此范围内
4 可以同时驱动2个直流电机，或者1个4线2相式步进电机。
5 PCB板尺寸：2.8cm*2.1cm 超小体积，适合组装
6 设有固定安装孔，直径：3mm
二 模块接口说明
【6P黑色弯排针说明】
1 VCC  外接2.5V-12V电压
2 GND  外接GND
3 IA1  外接单片机IO口
4 IB1  外接单片机IO口
5 IA2  外接单片机IO口
6 IB2  外接单片机IO口
【4P绿端子说明】
   1 OA1 OB1 接直流电机2个引脚，无方向
   2 OA2 OB2 接直流电机2个引脚，无方向
三 模块使用说明 
接通VCC，GND 模块电源指示灯亮  
IA1输入高电平，IB1输入低电平，【OA1 OB1】电机正转；
IA1输入低电平，IB1输入高电平，【OA1 OB1】电机反转；
IA2输入高电平，IB2输入低电平，【OA2 OB2】电机正转；
IA2输入低电平，IB2输入高电平，【OA2 OB2】电机反转；

* Created 4 October 2014
* By Jeffrey Sun

*/

#ifndef ERXL9110_H
#define ERXL9110_H

#include <Arduino.h>
#include <ERxIMotor.h>

#define L9110_LIB_VER "1.0"

class ERxL9110 : public ERxIMotor
{
public:
	/*
	The passed in pins will be set to be OUTPUT in the constructor.

	@constructor
	@param {unsigned char} a the pin index of Arduino which connects to the A1/A2 pin of L9110.
	@param {unsigned char} b the pin index of Arduino which connects to the B1/B2 pin of L9110.
	@param {bool} reverse indicate if reverse the direction.
	*/
	ERxL9110(unsigned char a, unsigned char b, bool reverse = false);

public:
	virtual void forward(unsigned int speed = 0);
	virtual void backward(unsigned int speed = 0);
	virtual void stop();

private:
	unsigned char 	m_a; 
	unsigned char 	m_b;
	bool			m_reverse;
};
#endif // ERXL9110_H
