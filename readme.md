# One Wire Driver for the PIC16F54 7 segments display

The Driver control the display over One Wire bus. The driver can set up the luminosity and shut-down the display. When the driver shutdown the display, the display can only be startup again from poweron or pushing the disable/enable-pin down. 

The "PIC16F54 7 segments display":

- 3 x 7 segments common cathode (5611AS)
- 1 x PIC16F54 Micro-Controller
- 3 x 2n7002 Transistors

## Features

- One wire bus
- Disable/Enable Pin
- 3 characters

## Schematic

The [Schematic](documents/images/pic16f54-7-segments-display-schematic.pdf) and the 
[board](documents/images/pic16f54-7-segments-display_pcb.pdf) files 

## One Wire Protocol


#define A ( 1 << 0)
#define B ( 1 << 1)
#define C ( 1 << 2)
#define D ( 1 << 3)
#define E ( 1 << 4)
#define F ( 1 << 5)
#define G ( 1 << 6)
#define DP ( 1 << 7)

https://www.electronics-tutorials.ws/blog/7-segment-display-tutorial.html