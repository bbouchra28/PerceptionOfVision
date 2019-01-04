#include <avr/io.h>

#include "usart.h"

void USART_Init( unsigned int ubrr )
{
  // Set baud rate
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
  // Enable receiver and transmitter
  UCSR0B = (1<<RXEN)|(1<<TXEN);
  // Set frame format: 8data, 2stop bit
  UCSR0C = (1<<USBS)|(3<<UCSZ0);
}

void USART_Transmit( unsigned char data )
{
  // Wait for empty transmit buffer
  while ( !( UCSR0A & (1<<UDRE)) );
  // Put data into buffer, sends the data
  UDR0 = data;
}

unsigned char USART_Receive( void )
{
  // Wait for data to be received
  while ( !(UCSR0A & (1<<RXC)) );
  // Get and return received data from buffer
  return UDR0;
}

int USART_Available( void )
{
  return (UCSR0A & (1<<RXC));
}

void USART_print(unsigned char* buf)
{
  while(*buf!='\0'){
    USART_Transmit(*buf);
    ++buf;
  }
}

int USART_ReceiveString(unsigned char* buf, int n)
{
  int i = 0;
  unsigned char byte;
  do{
    byte = USART_Receive();
    buf[i] = byte;
    i++;
  }
  while(byte!='\n' && i<n-1);
  buf[i] = '\0';
  return i;
}