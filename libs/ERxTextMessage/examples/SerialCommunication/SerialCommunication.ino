#include <ERxTextMessage.h>


ERxTextMessage textMsg(&Serial);

void setup() {

	Serial.begin(9600);

	textMsg.print("Arduino server is connected\r\n");
	
}

void loop() {

	if(textMsg.available())
	{
		textMsg.print("250 ok print the received message\r\n");

		while(textMsg.available())
			textMsg.write((uint8_t)textMsg.read());

		textMsg.invalidate();

	}
}