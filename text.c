#include <util/delay.h>
#include "text.h"
#include "led.h"

// Array of arrays of 5 Bytes for each representation of an ASCII character stored in the Program Memory (Flash)
// The macro "PROGMEM" is used to store the characters array in flash
const char characters[96][5] PROGMEM = {
 {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}, // space
 {0b00000000,0b00000000,0b01001111,0b00000000,0b00000000}, // !
 {0b00000000,0b00000111,0b00000000,0b00000111,0b00000000}, // "
 {0b00010100,0b01111111,0b00010100,0b01111111,0b00010100}, // #
 {0b00100100,0b00101010,0b01111111,0b00101010,0b00010010}, // $
 {0b00100011,0b00010011,0b00001000,0b01100100,0b01100010}, // %
 {0b00110110,0b01001001,0b01010101,0b00100010,0b01010000}, // &
 {0b00000000,0b00000101,0b00000011,0b00000000,0b00000000}, // '
 {0b00000000,0b00011100,0b00100010,0b01000001,0b00000000}, // (
 {0b00000000,0b01000001,0b00100010,0b00011100,0b00000000}, // )
 {0b00010100,0b00001000,0b00111110,0b00001000,0b00010100}, // *
 {0b00001000,0b00001000,0b00111110,0b00001000,0b00001000}, // +
 {0b00000000,0b01010000,0b00110000,0b00000000,0b00000000}, // ,
 {0b00001000,0b00001000,0b00001000,0b00001000,0b00001000}, // -
 {0b00000000,0b01100000,0b01100000,0b00000000,0b00000000}, // .
 {0b00100000,0b00010000,0b00001000,0b00000100,0b00000010}, // /
 {0b00111110,0b01010001,0b01001001,0b01000101,0b00111110}, // 0
 {0b00000000,0b01000010,0b01111111,0b01000000,0b00000000}, // 1
 {0b01000010,0b01100001,0b01010001,0b01001001,0b01000110}, // 2
 {0b00100001,0b01000001,0b01000101,0b01001011,0b00110001}, // 3
 {0b00011000,0b00010100,0b00010010,0b01111111,0b00010000}, // 4
 {0b00100111,0b01000101,0b01000101,0b01000101,0b00111001}, // 5
 {0b00111100,0b01001010,0b01001001,0b01001001,0b00110000}, // 6
 {0b00000011,0b01110001,0b00001001,0b00000101,0b00000011}, // 7
 {0b00110110,0b01001001,0b01001001,0b01001001,0b00110110}, // 8
 {0b00000110,0b01001001,0b01001001,0b00101001,0b00011110}, // 9
 {0b00000000,0b01101100,0b01101100,0b00000000,0b00000000}, // :
 {0b00000000,0b01010110,0b00110110,0b00000000,0b00000000}, // ;
 {0b00001000,0b00010100,0b00100010,0b01000001,0b00000000}, // <
 {0b00010100,0b00010100,0b00010100,0b00010100,0b00010100}, // =
 {0b00000000,0b01000001,0b00100010,0b00010100,0b00001000}, // >
 {0b00000010,0b00000001,0b01010001,0b00001001,0b00000110}, // ?
 {0b00110010,0b01001001,0b01111001,0b01000001,0b00111110}, // @
 {0b01111110,0b00010001,0b00010001,0b00010001,0b01111110}, // A
 {0b01111111,0b01001001,0b01001001,0b01001001,0b00111110}, // B
 {0b00111110,0b01000001,0b01000001,0b01000001,0b00100010}, // C
 {0b01111111,0b01000001,0b01000001,0b01000001,0b00111110}, // D
 {0b01111111,0b01001001,0b01001001,0b01001001,0b01001001}, // E
 {0b01111111,0b00001001,0b00001001,0b00001001,0b00000001}, // F
 {0b00111110,0b01000001,0b01001001,0b01001001,0b00111010}, // G
 {0b01111111,0b00001000,0b00001000,0b00001000,0b01111111}, // H
 {0b01000001,0b01000001,0b01111111,0b01000001,0b01000001}, // I
 {0b00110000,0b01000001,0b01000001,0b00111111,0b00000001}, // J
 {0b01111111,0b00001000,0b00010100,0b00100010,0b01000001}, // K
 {0b01111111,0b01000000,0b01000000,0b01000000,0b01000000}, // L
 {0b01111111,0b00000010,0b00001100,0b00000010,0b01111111}, // M
 {0b01111111,0b00000100,0b00001000,0b00010000,0b01111111}, // N
 {0b00111110,0b01000001,0b01000001,0b01000001,0b00111110}, // O
 {0b01111111,0b00001001,0b00001001,0b00001001,0b00000110}, // P
 {0b00111110,0b01000001,0b01010001,0b00100001,0b01011110}, // Q
 {0b01111111,0b00001001,0b00001001,0b00011001,0b01100110}, // R
 {0b01000110,0b01001001,0b01001001,0b01001001,0b00110001}, // S
 {0b00000001,0b00000001,0b01111111,0b00000001,0b00000001}, // T
 {0b00111111,0b01000000,0b01000000,0b01000000,0b00111111}, // U
 {0b00001111,0b00110000,0b01000000,0b00110000,0b00001111}, // V
 {0b00111111,0b01000000,0b00111000,0b01000000,0b00111111}, // W
 {0b01100011,0b00010100,0b00001000,0b00010100,0b01100011}, // X
 {0b00000011,0b00000100,0b01111000,0b00000100,0b00000011}, // Y
 {0b01100001,0b01010001,0b01001001,0b01000101,0b01000011}, // Z
 {0b01111111,0b01000001,0b01000001,0b00000000,0b00000000}, // [
 {0b00000010,0b00000100,0b00001000,0b00010000,0b00100000}, // '\'
 {0b00000000,0b00000000,0b01000001,0b01000001,0b01111111}, // ]
 {0b00000100,0b00000010,0b00000001,0b00000010,0b00000100}, // ^
 {0b01000000,0b01000000,0b01000000,0b01000000,0b01000000}, // _
 {0b00000000,0b00000001,0b00000010,0b00000100,0b00000000}, // `
 {0b00100000,0b01010100,0b01010100,0b01010100,0b01111000}, // a
 {0b01111111,0b01001000,0b01000100,0b01000100,0b00111000}, // 0b
 {0b00111000,0b01000100,0b01000100,0b01000100,0b00100000}, // c
 {0b00111000,0b01000100,0b01000100,0b01001000,0b01111111}, // d
 {0b00111000,0b01010100,0b01010100,0b01010100,0b00011000}, // e
 {0b00001000,0b01111110,0b00001001,0b00000001,0b00000010}, // f
 {0b00001100,0b01010010,0b01010010,0b01010010,0b00111110}, // g
 {0b01111111,0b00001000,0b00000100,0b00000100,0b01111000}, // h
 {0b00000000,0b01000100,0b01111101,0b01000000,0b00000000}, // i
 {0b00100000,0b01000000,0b01000100,0b00111101,0b00000000}, // j
 {0b01111111,0b00010000,0b00101000,0b01000100,0b00000000}, // k
 {0b00000000,0b01000001,0b01111111,0b01000000,0b00000000}, // l
 {0b01111000,0b00000100,0b00001000,0b00000100,0b01111000}, // m
 {0b01111100,0b00001000,0b00000100,0b00000100,0b01111000}, // n
 {0b00111000,0b01000100,0b01000100,0b01000100,0b00111000}, // o
 {0b01111100,0b00010100,0b00010100,0b00010100,0b00001000}, // p
 {0b00001000,0b00010100,0b00010100,0b01111100,0b00000000}, // q
 {0b01111100,0b00001000,0b00000100,0b00000100,0b00001000}, // r
 {0b01001000,0b01010100,0b01010100,0b01010100,0b00100000}, // s
 {0b00000100,0b00111111,0b01000100,0b01000000,0b00100000}, // t
 {0b00111100,0b01000000,0b01000000,0b00100000,0b01111100}, // u
 {0b00011100,0b00100000,0b01000000,0b00100000,0b00011100}, // v
 {0b00111100,0b01000000,0b00110000,0b01000000,0b00111100}, // w
 {0b01000100,0b00101000,0b00010000,0b00101000,0b01000100}, // x
 {0b00001100,0b01010000,0b01010000,0b01010000,0b00111100}, // y
 {0b01000100,0b01100100,0b01010100,0b01001100,0b01000100}, // z
 {0b00000000,0b00001000,0b00110110,0b01000001,0b00000000}, // {
 {0b00000000,0b00000000,0b01111111,0b00000000,0b00000000}, // |
 {0b00000000,0b01000001,0b00110110,0b00001000,0b00000000}, // }
 {0b00001000,0b00000100,0b00000100,0b00001000,0b00000100} // ~
}; // characters[95]

void displayChar(char c)
{
  for (int i = 0; i <5; i++)
  {
    LED_transmit(characters[c - 32][i]);
    _delay_us(1);
  }
  LED_transmit(0x00);
}

void displayString(char* s)
{
  for (int i = 0; i<=strlen(s); i++)
  {
    displayChar(s[i]);
  }
}
