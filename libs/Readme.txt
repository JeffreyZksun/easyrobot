
All the c and c++ source files under ERCores and  ArduinoCores will be compiled.

============================Subfolders==============================================
ERCores		: Library files added special for EasyRobot project
Arduino		: Library files copied from <arduino-1.0>\hardware\arduino\
	Updated Date: 12/17/2011
Other subfolders are the arduino libs.

Servo
	URL: Copy from arduino-1.0
	Features: Generate the servo PWM siginal to control the servo.
	Updated Date: 12/17/2011

ps2 
	URL£º http://www.arduino.cc/playground/ComponentLib/Ps2mouse
	Features: Encapsulate the PS2 protocol, communicate with PS2 device, such as mouse and keyboard.
	Updated Date: 11/10/2010
	
PS2Keyboard
	URL: http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html
	Features: Communicate with PS2 keyboard.
	Updated Date: 11/10/2010

NECIRrcv
	URL: http://arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1210243556Link
	Features: Encapsulate IR protocol, receive the signal from NEC IR device.
	Updated Date: 11/10/2010	

IRremote
	URL: http://www.pjrc.com/teensy/td_libs_IRremote.html
	Features: Encapsulate IR protocol, communicate with IR device. Support many kinds of IR devices.
	Updated Date: 11/10/2010

Time
	URL: http://www.arduino.cc/playground/Code/Time
	Features: Keeps the time with the format year, month, date, hour, minute, second. It also supports sync time with external modules. This is implement by a call back function.
	Updated Date: 18/4/2011

Ethernet
	URL: Copy from arduino-1.0
	Features: Encapsulate the TCP/IP transmission.
	Updated Date: 12/17/2011

Wire
	URL: Copy from arduino-1.0
	Features: I2C communication.
	Updated Date:	12/17/2011

SPI
	URL: Copy from arduino-1.0
	Features:
	Updated Date:	12/17/2011

DHT11
	URL: http://arduino.cc/playground/Main/DHT11Lib
	Features: Read the temperature and humidity data from the DHT11 sensor
	Updated Date:	12/18/2011

WebServer
	URL: https://github.com/sirleech/Webduino
	Features: Implement the web server.
	Updated Date: 6/4/2012

AccelStepper
	URL: http://www.airspayce.com/mikem/arduino/AccelStepper/
	Features: Drive the easy driver.
	Updated Date: 8/20/2013

XXXX
	URL:
	Features:
	Updated Date:

===========================File Structure of the libs==========================================
The file system structure of libs in this folder should keep consistent with the subfolder libraries under the Ardunio IDE installation path. So that the libs can be exchanged freely.
The details regarding the libary format can be found at http://arduino.cc/en/Hacking/LibraryTutorial

The the LED library as the example to thow the file structure under Arduino library.
	arduino-xxxx
	|--libraries
		|--LED
			|--examples
			|   |--Blink
			|   |   |--Blink.ino (The file name should be same as the folder)
			|   |--Loop
			|       |--Loop.ino
			|--keywords.txt
			|--LED.cpp (The file name should be same as the folder)
			|--LED.h

For the EasyRobot project, the examples and keywords.txt are not necessary.