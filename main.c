/*
 * File:   main.c
 * Author: pimi
 *
 * Created on October 3, 2019, 8:27 AM
 */

#include <stdint.h>
#include <xc.h>

// CONFIG
#pragma config OSC = RC  // Oscillator selection bits (XT oscillator)
#pragma config WDT = OFF // Watchdog timer enable bit (WDT disabled)
#pragma config CP = OFF  // Code protection bit (Code protection off)

enum states { IDLE, WAIT_FALLING_EDGE, WAIT_DATA, WAIT_HIGH_PULS };
// static uint8_t en = 0;

void main(void) {

  PORTA = 0x00;
  TRISA = 0xF8;

  PORTB = 0x00;
  TRISB = 0x00;

  uint8_t segments[4] = {0x00, 0xff, 0xff, 0xff};
  volatile uint8_t digit = 0;
  volatile enum states state = IDLE;
  volatile uint8_t count = 0;
  volatile uint8_t char_count = 15;
  volatile uint16_t character = 0;
  volatile uint8_t update_display = 0x00;

  while (1) {
    if (update_display) {
      if (update_display == (segments[0] & 0x3f))
        PORTB = 0x00;
      update_display--;
    } else {
      PORTB = 0x00;
      PORTA = (1 << digit);
      PORTB = segments[digit + 1];
      digit++;
      update_display = 0x3f;
    }

    if (digit == 3)
      digit = 0;
    if (segments[0] & 0x80) {
      PORTB = 0x00;
      SLEEP();
    }

    switch (state) {
    case IDLE:
      if (PORTAbits.RA3 == 0) {
        character = 0;
        char_count = 15;
        count = 0x00;
        state = WAIT_DATA;
      }

      break;
    case WAIT_FALLING_EDGE:
      if (count == 0x3f) {
        state = IDLE;
      } else {
        if (char_count == 0) {
          segments[(character >> 8) & 0x03] = character & 0xff;
          state = IDLE;
        }

        if (PORTAbits.RA3 == 0) {
          character = character << 1;
          char_count--;
          count = 0x08;
          state = WAIT_DATA;
        }

        count++;
      }
      break;
    case WAIT_HIGH_PULS:
      if (PORTAbits.RA3 != 0) {
        state = WAIT_FALLING_EDGE;
      } else {

        if (count == 0x4f) {
          state = IDLE;
        } else {
          count++;
        }
      }

      break;
    case WAIT_DATA:
      if (count == 0) {
        // PORTB ^= 0xff;
        if (PORTAbits.RA3 != 0) {
          character++;
          state = WAIT_FALLING_EDGE;
        } else {
          state = WAIT_HIGH_PULS;
        }

      } else {
        count--;
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
