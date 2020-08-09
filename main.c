/*
 * File:   main.c
 * Author: pimi
 *
 * Created on October 3, 2019, 8:27 AM
 */

#include "crc.h"
#include <stdint.h>
#include <xc.h>

// CONFIG
#pragma config OSC = RC  // Oscillator selection bits (XT oscillator)
#pragma config WDT = OFF // Watchdog timer enable bit (WDT disabled)
#pragma config CP = OFF  // Code protection bit (Code protection off)

enum states {
  IDLE,
  WAIT_FALLING_EDGE,
  WAIT_DATA,
  WAIT_HIGH_PULS,
  EXTRACT_BYTE
};

struct {
  uint8_t bit : 4;
  uint8_t character : 4;
} count;

void main(void) {

  PORTA = 0x00;
  TRISA = 0xF8;

  PORTB = 0x00;
  TRISB = 0x00;

  uint8_t segments[4] = {0x00, 0xff, 0xff, 0xff};
  uint8_t registers[3] = {0x00, 0x00, 0x00};
  volatile uint8_t digit = 0;
  volatile enum states state = IDLE;
  volatile uint8_t loop_count = 0;
  count.bit = 7;
  count.character = 0;
  volatile uint8_t character = 0;
  volatile uint8_t update_display = 0x00;

  while (1) {
    if (segments[0] & 0x80) {
      PORTB = 0x00;
      SLEEP();
    }

    if (segments[0] & 0x40) {
      PORTB = 0x00;
    } else if (update_display) {
      if (update_display == (segments[0] & 0x3f)) {
        PORTB = 0x00;
      }
      update_display--;
    } else {
      PORTB = 0x00;
      PORTA = (1 << digit);
      PORTB = segments[digit + 1];
      digit++;
      update_display = 0x3F;
    }

    if (digit == 3)
      digit = 0;

    switch (state) {
    case IDLE:
      if (PORTAbits.RA3 == 0) {
        character = 0;
        count.bit = 7;
        loop_count = 0x08;
        state = WAIT_DATA;
      }

      break;
    case WAIT_FALLING_EDGE:
      if (loop_count == 0x00) {
        count.character = 0;
        state = IDLE;
      } else {
        if (count.bit == 0) {
          state = EXTRACT_BYTE;
        }

        if (PORTAbits.RA3 == 0) {
          character = character << 1;
          count.bit--;
          loop_count = 0x08;
          state = WAIT_DATA;
        }

        loop_count--;
      }
      break;

    case EXTRACT_BYTE:
      if (count.character != ((uint8_t)sizeof(registers))) {
        registers[count.character] = character;
        count.character++;
      }
      if (count.character == ((uint8_t)sizeof(registers))) {
        segments[registers[0]] = registers[1];
        count.character = 0;
      }
      state = IDLE;
      break;

    case WAIT_HIGH_PULS:
      if (PORTAbits.RA3 != 0) {
        loop_count = 0x30;
        state = WAIT_FALLING_EDGE;
      } else {

        if (loop_count == 0x00) {
          count.character = 0;
          state = IDLE;
        } else {
          loop_count--;
        }
      }

      break;
    case WAIT_DATA:
      if (loop_count == 0) {
        if (PORTAbits.RA3 != 0) {
          character++;
          loop_count = 0x50;
          state = WAIT_FALLING_EDGE;
        } else {
          loop_count = 0x20;
          state = WAIT_HIGH_PULS;
        }

      } else {
        loop_count--;
      }

      break;
    default:
      character = 0;
      state = IDLE;
      break;
    }
  } // end while
  return;
}
