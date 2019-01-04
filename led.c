#include <avr/io.h>
#include <util/delay.h>

#include "led.h"


void SPI_MasterInit(void)
{
  // Set MOSI and SCK output, all others input
  DDRB = (1<<DDB2)|(1<<DDB1);
  // Enable SPI, Master, set clock rate fck/16
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

void SPI_MasterTransmit(char cData)
{
  // Start transmission
  SPDR = cData;
  // Wait for transmission complete
  while(!(SPSR & (1<<SPIF)));
}
// __________________________________________________________

/*int led_status = 0;

void LED_on(char led)
{
  led_status |= _BV(led);
}

void LED_off(char led)
{
  led_status &= ~_BV(led);
}

void LED_all_off()
{
  led_status = 0;
}

void LED_all_on()
{
  led_status = 0xffff;
}*/

void LED_transmit(int led_status)
{
  char data = (led_status>>8 & 0xff);
  SPI_MasterTransmit(data);
  data = (led_status & 0xff);
  SPI_MasterTransmit(data);

  PORTE |= _BV(DDE5);
  _delay_us(1);
  PORTE &= ~_BV(DDE5);
}