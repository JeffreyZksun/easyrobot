/*
  PS2Keyboard.h - PS2Keyboard library
  Copyright (c) 2007 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

  ** Mostly rewritten Paul Stoffregen <paul@pjrc.com>, June 2010
  ** Modified for use with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
  ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef PS2Keyboard_h
#define PS2Keyboard_h

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

// Every call to read() returns a single byte for each
// keystroke.  These configure what byte will be returned
// for each "special" key.  To ignore a key, use zero.
#define PS2_TAB         9
#define PS2_ENTER       13
#define PS2_BACKSPACE   127
#define PS2_ESC         27
#define PS2_INSERT      0
#define PS2_DELETE      127
#define PS2_HOME        0
#define PS2_END         0
#define PS2_PAGEUP      25
#define PS2_PAGEDOWN    26
#define PS2_UPARROW     11
#define PS2_LEFTARROW   8
#define PS2_DOWNARROW   10
#define PS2_RIGHTARROW  21
#define PS2_F1          0
#define PS2_F2          0
#define PS2_F3          0
#define PS2_F4          0
#define PS2_F5          0
#define PS2_F6          0
#define PS2_F7          0
#define PS2_F8          0
#define PS2_F9          0
#define PS2_F10         0
#define PS2_F11         0
#define PS2_F12         0
#define PS2_SCROLL      0

/* Previous versions of this library returned a mix of ascii characters
 * and raw scan codes (if no ASCII character could be found).  There was
 * no way to determine if a byte was ascii or a raw scan code.  With
 * version 2.0, only ASCII is returned, plus the special bytes above.
 * These raw scan codes are never returned by read() anymore.
 *
 * PS2 keyboard "make" codes to check for certain keys.
 */
#define PS2_KC_BREAK  0xf0
#define PS2_KC_ENTER  0x0d
#define PS2_KC_ESC    0x76
#define PS2_KC_KPLUS  0x79
#define PS2_KC_KMINUS 0x7b
#define PS2_KC_KMULTI 0x7c
#define PS2_KC_NUM    0x77
#define PS2_KC_BKSP   0x66


#include "binary.h"
typedef uint8_t boolean;
typedef uint8_t byte;

/**
 * Purpose: Provides an easy access to PS2 keyboards
 * Author:  Christian Weichel
 */
class PS2Keyboard {
  public:
  	/**
  	 * This constructor does basically nothing. Please call the begin(int,int)
  	 * method before using any other method of this class.
  	 */
    PS2Keyboard();
    
    /**
     * Starts the keyboard "service" by registering the external interrupt.
     * setting the pin modes correctly and driving those needed to high.
     * The propably best place to call this method is in the setup routine.
     */
    static void begin(uint8_t dataPin, uint8_t irq_pin);
    
    /**
     * Returns true if there is a char to be read, false if not.
     */
    static bool available();
    
    /**
     * Returns the char last read from the keyboard.
     * If there is no char availble, -1 is returned.
     */
    static int read();
};

// interrupt pins for known boards
#ifndef CORE_INT0_PIN   // if not already defined by core (eg, Teensy)
#if defined(__AVR_ATmega1280__) // Arduino Mega
#define CORE_INT0_PIN  2
#define CORE_INT1_PIN  3
#define CORE_INT2_PIN  21
#define CORE_INT3_PIN  20
#define CORE_INT4_PIN  19
#define CORE_INT5_PIN  18
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__) // Sanguino
#define CORE_INT0_PIN  10
#define CORE_INT1_PIN  11
#define CORE_INT2_PIN  2
#else  // Arduino Duemilanove, Diecimila, LilyPad, Mini, Fio, etc...
#define CORE_INT0_PIN  2
#define CORE_INT1_PIN  3
#endif
#endif

#endif
