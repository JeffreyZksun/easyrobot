/**************************************************************************************
Functionality:	
<Describe the main functionality of the code in this file.>

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
#include <Arduino.h>
#include "SPI.h"
#include "Ethernet.h"
#include <HardwareSerial.h>

// Define the macros to configure the web server library.
#define WEBDUINO_SUPRESS_SERVER_HEADER 1
#define WEBDUINO_AUTH_REALM "My Clould"
#define WEBDUINO_FAVICON_DATA ""
#include "WebServer.h"

/* CHANGE THIS TO YOUR OWN UNIQUE VALUE.  The MAC number should be
 * different from any other devices on your network or you'll have
 * problems receiving packets. */
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

/* CHANGE THIS TO MATCH YOUR HOST NETWORK.  Most home networks are in
 * the 192.168.0.XXX or 192.168.1.XXX subrange.  Pick an address
 * that's not in use and isn't going to be automatically allocated by
 * DHCP from your router. */
static uint8_t ip[] = { 192, 168, 1, 210 };

/* This creates an instance of the webserver.  By specifying a prefix
 * of "", all pages will be at the root of the server. */
#define PREFIX ""
WebServer webserver(PREFIX, 80);

void irCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);
void rootPageCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);

void setup()
{
	Serial.begin(9600);

  // setup the Ethernet library to talk to the Wiznet board
  Ethernet.begin(mac, ip);

  /* register our default command (activated with the request of
   * http://x.x.x.x/
   */

  // The verb is the string between the / and ?.
  // For example "GET /ir/index.htm?code=0x521 HTTP/1.1". The verb is "ir/index.htm"
  // The web server compares the whole verb string when dispatch the command.
  webserver.setDefaultCommand(&rootPageCmd); // GET / HTTP/1.1
  webserver.addCommand("ir/", &irCmd); // GET /ir/ HTTP/1.1

  /* start the server to wait for connections */
  webserver.begin();
}

void loop()
{
	char buff[64];
	int len = 64;

	// process incoming connections one at a time forever
	webserver.processConnection(buff, &len);
}

void ResponseIRPageBody(WebServer &server);
void ResponseRootPageBody(WebServer &server);
void DispatchIRCommand(const char* pIRCode);

// http://www.mysite.net/
void rootPageCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
	/* we don't output the body for a HEAD request */
	if (type == WebServer::GET)
	{
		server.httpSuccess();

		ResponseRootPageBody(server);
	}
}

// http://www.mysite.net/ir/
void irCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      /* readPOSTparam returns false when there are no more parameters
       * to read from the input.  We pass in buffers for it to store
       * the name and value strings along with the length of those
       * buffers. */
      repeat = server.readPOSTparam(name, 16, value, 16);

      /* this is a standard string comparison function.  It returns 0
       * when there's an exact match.  We're looking for a parameter
       * named "buzz" here. */
      if (strcmp(name, "IR") == 0)
      {
		// dispatch the command
		DispatchIRCommand(value);
		break;
      }
    } while (repeat);
    
    // after processing the POST data, tell the web browser to reload
    // the page using a GET method. 
    server.httpSeeOther("/ir/");
    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" */
  server.httpSuccess();

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
	ResponseIRPageBody(server);
  }
}

void ResponseRootPageBody(WebServer &server)
{
	/* store the HTML in program memory using the P macro */
	P(message) = 
		"<html><head><title>MyCloud</title>"
			"</head>"
		"<body><P>"
			"<p>"
			"<a href='/ir/'>Cloud controller</a>"
			"</P>"
		"</P></body></html>";

	server.printP(message);
}

void ResponseIRPageBody(WebServer &server)
{
	/* store the HTML in program memory using the P macro */
	P(message) = 
	"<!doctype html>" "<html><head><title>Cloud controller</title>" "<style type='text/css'>" "table {    margin-left: auto;" "margin-right: auto; width: 180px;}" "tr{text-align: center;}" "td{width: 60px; padding-left: 2px;padding-bottom: 7px;}" "button {" "display: inline-block;" "background: -webkit-gradient(linear, left top, left bottom, color-stop(0%,#f1da36), color-stop(100%,#fcf088));" "background: -moz-linear-gradient(center top, #f1da36 0%, #fcf088 100%);" "-webkit-box-shadow: 0px 0px 5px #777777, 0px 0px 2px #333333 inset;" "-moz-box-shadow: 0px 0px 5px #777777, 0px 0px 2px #333333 inset;" "box-shadow: 0px 0px 5px #777777, 0px 0px 2px #333333 inset;" "-webkit-border-radius: 30px;" "-moz-border-radius: 30px;" "border-radius: 30px;" "padding: 10px 15px;" "border-color: #ffffff;" "border-width: 1px;" "border-style: solid;" "font-size: 16px;" "color: #000000;" "font-weight: bold;" "}" "button:hover {" "background: -webkit-gradient(linear, left top, left bottom, color-stop(0%,#eeff07), color-stop(100%,#fcf088));" "background: -moz-linear-gradient(center top, #eeff07 0%, #fcf088 100%);" "}" "button:active {" "-webkit-box-shadow: 0px 0px 5px #333333 inset;" "-moz-box-shadow: 0px 0px 5px #333333 inset;" "box-shadow: 0px 0px 5px #333333 inset;" "}" "</style>" "</head>" "<body><P>" "<form action='/ir/' method='POST'>" "<table>" "<tr>" "<td><button name='IR' value='0xFF629D'>PWR</button></td>" "<td>&nbsp;</td>" "<td>&nbsp;</td>" "</tr>" "<tr>" "<td><button name='IR' value='0xFF807F'>1</button></td>" "<td><button name='IR' value='0xFF40BF'>2</button></td>" "<td><button name='IR' value='0xFFC03F'>3</button></td>" "</tr>" "<tr>" "<td><button name='IR' value='0xFF20DF'>4</button></td>" "<td><button name='IR' value='0xFFA05F'>5</button></td>" "<td><button name='IR' value='0xFF609F'>6</button></td>" "</tr>" "<tr>" "<td><button name='IR' value='0xFFE01F'>7</button></td>" "<td><button name='IR' value='0xFF10EF'>8</button></td>" "<td><button name='IR' value='0xFF906F'>9</button></td>" "</tr>" "<tr>" "<td><button name='IR' value='0xFF42BD'>-/-- </button></td>" "<td><button name='IR' value='0xFF00FF'>0</button></td>" "<td><button name='IR' value='0xFF827D'>Back</button></td>" "</tr>" "<tr>" "<td><button name='IR' value='0xFFF00F'>VOL+</button></td>" "<td><button name='IR' value='0xFF32CD'>Mute</button></td>" "<td><button name='IR' value='0xFF30CF'>CH+</button></td>" "</tr>" "<tr>" "<td><button name='IR' value='0xFF5AA5'>VOL-</button></td>" "<td>&nbsp;</td>" "<td><button name='IR' value='0xFF9867'>CH-</button></td>" "</tr>" "</table>" "</form>" "</P></body></html>";

	server.printP(message);
}

void DispatchIRCommand(const char* pIRCode)
{
	Serial.print(0x50);
	Serial.print(' ');
	Serial.print(0x02);
	Serial.print(' ');
	Serial.print(0x03);
	Serial.print(' ');
	Serial.print(0x20);
	Serial.print(' ');
	Serial.print(pIRCode);
	Serial.print("\r\n");
}