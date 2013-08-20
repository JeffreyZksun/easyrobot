/***********************************************************************
Defines all the macros used by the instruction frame protocol.

************************************************************************/

#pragma once

#ifndef PROTOCOLMACROS_H
#define PROTOCOLMACROS_H

/********************************************************************************************
The protocol is shared by all the inter-connected devices, such as PC, MCU, etc.

The format of the data frame. The maximum length is MAX_FAME_LENGTH.
+------------+-----------+------------+----------------+-----------+-----------+---------+-------------+
|  0  |   1  |     2     |     3      |        4       |     5     |        ......                     |
+------------+-----------+------------+----------------+-----------+-----------+---------+-------------+
|  Header(2) | Source    | Target     | Instruction(1) | Param     | Parameter |  CRC(2) | Footer (2)  |
|			 | Device (1)| Device (1) |				   | Length(1) | (0-117)   |         |             |
+------------+-----------+------------+----------------+-----------+-----------+---------+-------------+
|0x41 |	0x41 |    0x00   |    0x00    |                |           |           | Not use | 0x0D | 0x0A | 
+------------+-----------+------------+----------------+-----------+-----------+---------+-------------+

The source device and target device will be used when communicate with multi-devices.

********************************************************************************************/

#define PROTOCOL_VERSION 1


#define RING_BUFFER_SIZE 128

#define MAX_FAME_LENGTH 127				// RING_BUFFER_SIZE -1
#define FRAEM_CONTROL_CHARS_LENGTH  10	// 10 = (2 + 1 + 1 + 1 + 1 + 2 + 2)
#define MAX_PARAMETER_LENGTH 117		// MAX_FAME_LENGTH - FRAEM_CONTROL_CHARS_LENGTH

// Data frame index
#define HEADER_0_INDEX			0
#define HEADER_1_INDEX			1
#define SOURCE_DEVICE_INDEX		2
#define TARGET_DEVICE_INDEX		3
#define INSTRUCTION_CMD_INDEX	4
#define PARA_LENGTH_INDEX		5
#define PARA_HEADER_INDEX		6



// Data frame value
#define HEADER_0 0x41 // 'A'
#define HEADER_1 0x41

#define DEVICE_BROADCAST 0x30  // '0'

#define EMPTY_CRC 0x5A // 'Z'

#define FOOTER_0 0x0D // Carriage-Return
#define FOOTER_1 0x0A // Line break

// Command macros.
#define INVALID_CMD 0x00

// Empty pointer
#ifndef NULL
#define  NULL 0
#endif

#endif
