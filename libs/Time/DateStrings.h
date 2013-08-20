/* DateStrings.h
 * Definitions for date strings for use with the Time library
 *
 * This header isn't included in the official library http://www.arduino.cc/playground/Code/Time
 * Just add it for the compile.
 * 
 */
#ifndef _DateStrings_h
#define _DateStrings_h

/* functions to return date strings */

char* monthStr(uint8_t month);
char* monthShortStr(uint8_t month);
char* dayStr(uint8_t day);
char* dayShortStr(uint8_t day);

#endif // _DateStrings_h
