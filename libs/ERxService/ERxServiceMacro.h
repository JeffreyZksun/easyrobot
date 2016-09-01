#pragma once
#ifndef ERX_SERVICE_MACRO_H
#define ERX_SERVICE_MACRO_H

// declare a static string
#define PROG(name)   static const char name[] PROGMEM

// host addresses
#define INVALID_ADDRESS 0
#define BROADCAST_ADDRESS 255

// standard END-OF-MESSAGE marker in ERxTextMessage protocol
#define CRLF "\r\n"

#endif // ERX_SERVICE_MACRO_H