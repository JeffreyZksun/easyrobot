#include <ERxCore.h>

/**************************************************************************************
Functionality:	


Input 192.168.1.177 to the IE to connect this web server.

The circuit:
<Describe the used circuit connection.>

More:
<Add more information regarding the hardware>

Occupied MCU resource:
<Indicate which resources are occupied by this functionality. 
When try to implement other functionalities, these resources shouldn't be used again.>

Author:		<Author>
Created:	<Date>

***************************************************************************************/

//=============Implement the robot functionalities in the functions below===============


#include <avr/pgmspace.h>

#include <Ethernet.h>
#include <SPI.h>
#include "ERxProtocol.h"
#include "SerialComHelper.h"

/************************************************************************/
//  CMD     PARAMETERS					COMMENTS                                                              
//  0x11		N/A					   RUN_FORWARD_CMD
//  0x12		N/A					   TURN_LEFT_CMD
//  0x13		N/A					   TRUN_RIGHT_CMD
//  0x14		N/A					   RUN_BACKFARD_CMD
//  0x15		N/A					   STOP_CMD
//  0x20		N/A					   QUERY_STATUS
//  0x21		ERFLOAT32			   REPLY_STATUS
/************************************************************************/

#define RUN_FORWARD_CMD		0x11
#define TURN_LEFT_CMD		0x12
#define TRUN_RIGHT_CMD		0x13
#define RUN_BACKFARD_CMD	0x14
#define STOP_CMD			0x15

#define QUERY_STATUS		0x20
#define REPLY_STATUS		0x21

//#define DEBUG

#define zap(x) if(x){free(x); x=0;}

#define refreshTIME 2000	// micro seconds

#define bufferSIZE 128

// Read Only strings
// Http protocol header
prog_char line_00[] PROGMEM = "HTTP/1.1 200 OK" ;
prog_char line_01[] PROGMEM = "Content-Type: text/html\n\n" ;

// Begin html,title
prog_char line_02[] PROGMEM = "<html><head>" ;
prog_char line_03[] PROGMEM = "<title>Robot Online</title>\n" ;

// Style
prog_char style_00[] PROGMEM = "<style type=\"text/css\">\n" ;
prog_char style_01[] PROGMEM = "	.buttonType\n" ;
prog_char style_02[] PROGMEM = "	{\n" ;
prog_char style_03[] PROGMEM = "		height: 65px; \n" ;
prog_char style_04[] PROGMEM = "		width: 65px;\n" ;
prog_char style_05[] PROGMEM = "		font-size: 13px;\n" ;
prog_char style_06[] PROGMEM = "	}\n" ;
prog_char style_07[] PROGMEM = "</style>\n" ;
// Javascript
prog_char javascript_00[] PROGMEM = "<script type='text/javascript'>\n" ; //03
prog_char javascript_01[] PROGMEM = "var xD;\n" ;
prog_char javascript_02[] PROGMEM = "var dB=new Array();\n" ;
prog_char javascript_03[] PROGMEM = "function fetch(I){\n" ;
prog_char javascript_04[] PROGMEM = "I = I + ',' + document.getElementById('UserPWD').value;\n";
prog_char javascript_05[] PROGMEM = "xD=getXdO();\n" ;
prog_char javascript_06[] PROGMEM = "if(xD==null){" ;
prog_char javascript_07[] PROGMEM = "alert('No AJAX'); return}\n" ;
prog_char javascript_08[] PROGMEM = "xD.onreadystatechange=stateChanged;" ;
prog_char javascript_09[] PROGMEM = "xD.open('post',I,true);" ;
prog_char javascript_10[] PROGMEM = "xD.send(null)}\n" ;
prog_char javascript_11[] PROGMEM = "function stateChanged(){\n" ;
prog_char javascript_12[] PROGMEM = "if(xD.readyState==4){" ;
prog_char javascript_13[] PROGMEM = "dB = xD.responseText.split(',');\n" ; 

prog_char javascript_14[] PROGMEM = "if(dB[" ; // Ai
prog_char javascript_15[] PROGMEM = "]){document.getElementById('D" ; // Ai
prog_char javascript_16[] PROGMEM = "').innerHTML=dB[" ; // Ai
prog_char javascript_17[] PROGMEM = "]}\n" ; 

prog_char javascript_18[] PROGMEM ="}}\n" ;
prog_char javascript_19[] PROGMEM = "function getXdO(){var xD=null;\n" ;
prog_char javascript_20[] PROGMEM = "try{" ;
prog_char javascript_21[] PROGMEM = "xD=new XMLHttpRequest()}\n" ;
prog_char javascript_22[] PROGMEM = "catch (e){\n" ;
prog_char javascript_23[] PROGMEM = "try{" ;
prog_char javascript_24[] PROGMEM = "xD=new ActiveXObject(\"Msxml2.XMLHTTP\")}\n" ;
prog_char javascript_25[] PROGMEM = "catch (e){" ;
prog_char javascript_26[] PROGMEM = "xD=new ActiveXObject(\"Microsoft.XMLHTTP\")}}" ;
prog_char javascript_27[] PROGMEM = "return xD}\n" ;
prog_char javascript_28[] PROGMEM = "function A(){" ;
prog_char javascript_29[] PROGMEM = "setTimeout('A()'," ; //32

prog_char javascript_30[] PROGMEM = ");" ;
prog_char javascript_31[] PROGMEM = "fetch('@')}\n" ;
prog_char javascript_32[] PROGMEM = "</script>\n" ; //35

// End head
prog_char line_04[] PROGMEM = "</head>\n" ;
// Begin body
prog_char line_05[] PROGMEM = "<body onload='A()'>\n" ; 
prog_char line_06[] PROGMEM = "<p style=\"text-align: center; font-size: 18px; font-weight: bold;\">    Welcome to Jeffrey Sun&#39;s remote robot onlone control center!</p>\n" ;

// Online control
prog_char control_00[] PROGMEM = "<table align=\"center\" style=\"width: 200px;\">\n";
prog_char control_01[] PROGMEM = "    <tr>\n";
prog_char control_02[] PROGMEM = "        <td style=\"text-align: center\">&nbsp;</td>\n";
prog_char control_03[] PROGMEM = "        <td style=\"text-align: center\">\n";
prog_char control_04[] PROGMEM = "            <input id=\"Forward\" type=\"button\" value=\"Forward\" onClick=\"fetch('F')\" class=\"buttonType\"/></td>\n";
prog_char control_05[] PROGMEM = "        <td style=\"text-align: center\">&nbsp;</td>\n";
prog_char control_06[] PROGMEM = "    </tr>\n";
prog_char control_07[] PROGMEM = "    <tr>\n";
prog_char control_08[] PROGMEM = "        <td style=\"text-align: center\">\n";
prog_char control_09[] PROGMEM = "            <input id=\"Left\" type=\"button\" value=\"Left\" onClick=\"fetch('L')\" class=\"buttonType\"/></td>\n";
prog_char control_10[] PROGMEM = "        <td style=\"text-align: center\">\n";
prog_char control_11[] PROGMEM = "            <input id=\"Stop\" type=\"button\" value=\"Stop\" onClick=\"fetch('S')\" class=\"buttonType\"/></td>\n";
prog_char control_12[] PROGMEM = "        <td style=\"text-align: center\">\n";
prog_char control_13[] PROGMEM = "            <input id=\"Right\" type=\"button\" value=\"Right\" onClick=\"fetch('R')\" class=\"buttonType\"/></td>\n";
prog_char control_14[] PROGMEM = "    </tr>\n";
prog_char control_15[] PROGMEM = "    <tr>\n";
prog_char control_16[] PROGMEM = "        <td style=\"text-align: center\">&nbsp;</td>\n";
prog_char control_17[] PROGMEM = "        <td style=\"text-align: center\" >\n";
prog_char control_18[] PROGMEM = "            <input id=\"Backward\" type=\"button\" value=\"Backward\" onClick=\"fetch('B')\" class=\"buttonType\"/></td>\n";
prog_char control_19[] PROGMEM = "        <td style=\"text-align: center\">&nbsp;</td>\n";
prog_char control_20[] PROGMEM = "    </tr>\n";
prog_char control_21[] PROGMEM = "		<tr>\n";
prog_char control_22[] PROGMEM = "			<td style=\"text-align: center\" colspan=\"3\">Password:<input id=\"UserPWD\" type=\"password\" style=\"width: 100px;\" /></td>\n";
prog_char control_23[] PROGMEM = "		</tr>\n";
prog_char control_24[] PROGMEM = "</table>\n";

// analog
prog_char analog_00[] PROGMEM = "<table align=\"center\" style=\"width: 200px;\">\n";
prog_char analog_01[] PROGMEM = "<tr>		<td >\n";
prog_char analog_02[] PROGMEM = "<a href=\"http://58.38.218.220:90\" target =\"_blank\">Open Live Video</a><br /><br />\n";
prog_char analog_03[] PROGMEM = "Battery Voltage(V)  <span id='D0'></span><br>\n" ; 

prog_char analog_04[] PROGMEM = "Analog " ; // Ai
prog_char analog_05[] PROGMEM = " <span id='D" ; // Ai
prog_char analog_06[] PROGMEM = "'></span><br>\n" ; 

prog_char analog_07[] PROGMEM = "		</td>\n";
prog_char analog_08[] PROGMEM = "	</tr>\n";
prog_char analog_09[] PROGMEM = "</table>\n";

// End body,html
prog_char line_07[] PROGMEM = "</body></html>\n" ;

PROGMEM const char *string_table[] = {
	line_00, line_01, line_02, line_03, line_04,
	line_05, line_06, line_07
};

PROGMEM const char *javascript_table[] = {
	javascript_00, javascript_01, javascript_02, javascript_03, javascript_04,
	javascript_05, javascript_06, javascript_07, javascript_08, javascript_09,
	javascript_10, javascript_11, javascript_12, javascript_13, javascript_14,
	javascript_15, javascript_16, javascript_17, javascript_18, javascript_19,
	javascript_20, javascript_21, javascript_22, javascript_23, javascript_24,
	javascript_25, javascript_26, javascript_27, javascript_28, javascript_29,
	javascript_30, javascript_31, javascript_32
};

PROGMEM const char *style_table[] = {
	style_00, style_01, style_02, style_03, style_04,
	style_05, style_06, style_07
};

PROGMEM const char *control_table[] = {
	control_00, control_01, control_02, control_03, control_04,
	control_05, control_06, control_07, control_08, control_09,
	control_10, control_11, control_12, control_13, control_14,
	control_15, control_16, control_17, control_18, control_19,
	control_20, control_21, control_22, control_23, control_24
};

PROGMEM const char *analog_table[] = {
	analog_00, analog_01, analog_02, analog_03, analog_04,
	analog_05, analog_06, analog_07, analog_08, analog_09
};

//Constants
byte mac[] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC };
byte servIP[] =  { 192, 168, 1, 177 };

Server server(80);

SerialComHelper comHepler;

#define passwordLength 4
const char* pValidPassword = "love";

ERFLOAT32 batterVoltage = 0.0;
// #########

void setup(){
	Serial.begin(9600);

	Ethernet.begin(mac,servIP);

	server.begin();

#ifdef DEBUG
	Serial.println("All started");
#endif

}

// #########

int getline(Client client, char *buffer){
	int i = 0;
	char c;
	buffer[i++] = client.read();
	buffer[i++] = client.read();
	while (buffer[i-2] != '\r' && buffer[i-1] != '\n' && client.available()) {
		c = client.read();
		if (i < bufferSIZE){
			buffer[i] = c;
			i ++;
		}
	}
	return i;
}

// #########

// send a web page complete with javascript
void makepage(Client client, char *buffer){

	for (int i = 0; i < 4; i++){ 
		strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i])));
		client.print(buffer);
	} 

	// style
	for (int i = 0; i < 8; i++){ 
		strcpy_P(buffer, (char*)pgm_read_word(&(style_table[i])));
		client.print(buffer);
	} 

	// javascript
	for (int i = 0; i < 14; i++){ 
		strcpy_P(buffer, (char*)pgm_read_word(&(javascript_table[i])));
		client.print(buffer);
	} 

	for (int i = 0 ; i < 6 ; i ++){
		strcpy_P(buffer, (char*)pgm_read_word(&(javascript_table[14])));
		client.print(buffer);
		client.print(i);
		strcpy_P(buffer, (char*)pgm_read_word(&(javascript_table[15])));
		client.print(buffer);
		client.print(i);
		strcpy_P(buffer, (char*)pgm_read_word(&(javascript_table[16])));
		client.print(buffer);
		client.print(i);
		strcpy_P(buffer, (char*)pgm_read_word(&(javascript_table[17])));
		client.print(buffer);
	}

	for (int i = 18; i < 30; i++){
		strcpy_P(buffer, (char*)pgm_read_word(&(javascript_table[i])));
		client.print(buffer);
	}

	client.print(refreshTIME);

	for (int i = 30; i < 33; i++){
		strcpy_P(buffer, (char*)pgm_read_word(&(javascript_table[i])));
		client.print(buffer);
	} 

	// End header, begin body
	for (int i = 4; i < 7; i++){
		strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i])));
		client.print(buffer);
	} 

	// control
	for (int i = 0; i < 25; i++){
		strcpy_P(buffer, (char*)pgm_read_word(&(control_table[i])));
		client.print(buffer);
	} 

	// analog
	for (int i = 0; i < 4; i++){
		strcpy_P(buffer, (char*)pgm_read_word(&(analog_table[i])));
		client.print(buffer);
	} 

	for (int i = 1 ; i < 6 ; i ++){
		strcpy_P(buffer, (char*)pgm_read_word(&(analog_table[4])));
		client.print(buffer);
		client.print(i);
		strcpy_P(buffer, (char*)pgm_read_word(&(analog_table[5])));
		client.print(buffer);
		client.print(i);
		strcpy_P(buffer, (char*)pgm_read_word(&(analog_table[6])));
		client.print(buffer);
	}

	for (int i = 7; i < 10; i++){
		strcpy_P(buffer, (char*)pgm_read_word(&(analog_table[i])));
		client.print(buffer);
	} 

	// End body, html
	strcpy_P(buffer, (char*)pgm_read_word(&(string_table[7])));
	client.print(buffer);
}

void SendQueryCommand()
{
	ERxParameterStream paraStream;
	comHepler.SendInstruction(&Serial, QUERY_STATUS, paraStream);
}

void GetBatteryVoltage()
{
	ERxInstructionFrame* pInstr = comHepler.GetFirstInstruction(&Serial);
	while(pInstr != NULL) 
	{
		if (pInstr->GetCommand() == REPLY_STATUS)
		{
			ERxParameterStream* pParaStream = pInstr->GetParameterStream();
			pParaStream->Read(&batterVoltage);

#ifdef DEBUG  
			Serial.println();
			Serial.print("Battery Voltage(V): ");
			Serial.print((double)batterVoltage);
			Serial.println();
#endif
		}

		pInstr = comHepler.GetFirstInstruction(&Serial);
	}
}
// #########

void webServ(Client client)
{
	char *buffer = (char *)malloc(bufferSIZE * sizeof(char *));

	while (client.connected()) {
		if (client.available()) {
			int len = getline(client, buffer);

#ifdef DEBUG  
			Serial.print(">>");
			int i = 0;
			while (buffer[i] >= 32){
				Serial.print(buffer[i]);
				i++;
			} 
			Serial.println();
#endif

			if (len > 2) {
				if (!memcmp(buffer,"GET / HTTP",10))
				{
					SendQueryCommand();
					makepage(client, buffer);
				}
				else if (!memcmp(buffer,"POST /",6)) // The command string from client. POST /F,love
				{
					if('@' == buffer[6]) // General query 
					{
						SendQueryCommand();
						GetBatteryVoltage();
						// send the value of each analog input pin - comma separated
						client.print((double)batterVoltage);
						for (int i = 1 ; i < 6 ; i ++)
						{
							client.print(',');
							client.print(analogRead(i));
						}
					}
					else if(!memcmp(&buffer[8], pValidPassword, passwordLength)) // Check the password for the operation command.
					{
						// look for returned values - crude but it works!
						switch(buffer[6])
						{
						case 'f':
						case 'F': 
							{
								ERxParameterStream dummyPara;
								comHepler.SendInstruction(&Serial, RUN_FORWARD_CMD, dummyPara);
							}
							break;
						case 'b':
						case 'B':
							{
								ERxParameterStream dummyPara;
								comHepler.SendInstruction(&Serial, RUN_BACKFARD_CMD, dummyPara);
							}
							break;
						case 'l':
						case 'L':
							{
								ERxParameterStream dummyPara;
								comHepler.SendInstruction(&Serial, TURN_LEFT_CMD, dummyPara);
							}
							break;
						case 'r':
						case 'R':
							{
								ERxParameterStream dummyPara;
								comHepler.SendInstruction(&Serial, TRUN_RIGHT_CMD, dummyPara);
							}
							break;
						case 's':
						case 'S':
							{
								ERxParameterStream dummyPara;
								comHepler.SendInstruction(&Serial, STOP_CMD, dummyPara);
							}
							break;
						}
					}

					
				}
				break;
			}
		}
	}

	// give the web browser time to receive the data
	delay(1);
	client.stop();
	zap(buffer);
}

// #########

// put everything in functions to make the loop clearer
void loop(){

	Client client = server.available();
	if (client) {
		webServ(client);
	}
}
