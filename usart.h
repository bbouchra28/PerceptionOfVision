#ifndef _USART_H_
#define _USART_H_

#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1

void USART_Init( unsigned int ubrr );
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
int USART_Available( void );
void USART_print(unsigned char* buf);
int USART_ReceiveString(unsigned char* buf, int n);

#endif