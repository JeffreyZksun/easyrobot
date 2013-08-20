#pragma once
#ifndef ERX_COMMAND_ID_H
#define ERX_COMMAND_ID_H


// 0x00 - 0x0F  System Service
#define SYS_INVALID_COMMAND 0x00
#define SYS_HELP 0x01
#define SYS_RESULT 0x02

// 0x10 - 0x1F Pachube Service
#define PACHUBE_PUT 0x10
#define PACHUBE_GET 0x11

// 0x20 - 0x2F IR controller Service
#define IR_SEND_NEC 0x20
#define IR_SEND_SONY 0x21
#define IR_SEND_RC5 0x22
#define IR_SEND_RC6 0x23
#define IR_DECODE 0x24

// 0x30 - 0x3F IO Service
#define IO_SET_INPUT 0x30
#define IO_SET_OUTPUT 0x31
#define IO_SET_VALUE 0x32
#define IO_GET_VALUE 0x33

// 0x40 - 0x4F Temperature and humidity detection service
#define TH_GET 0x40

// 0x50 - 0x5F Message router service
#define MR_SEND_TO 0x50


#endif // ERX_COMMAND_ID_H