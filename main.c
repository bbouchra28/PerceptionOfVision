#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

volatile unsigned int timePerCycle = 10000; // ms
volatile unsigned long timeCycle = 0; // ms

// ___________________________________________________
#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1

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

void USART_print(unsigned char* buf){
  while(*buf!='\0'){
    USART_Transmit(*buf);
    ++buf;
  }
}

void USART_ReceiveString(unsigned char* buf, int n){
  int i = 0;
  unsigned char byte;
  do{
    byte = USART_Receive();
    buf[i] = byte;
    i++;
  }
  while(byte!='\n' && i<n-1);
  buf[i] = '\0';
}

// __________________________________________________________
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
#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (F_CPU / 1000L) )
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))
// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)
// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

unsigned long timer0_overflow_count;
unsigned long timer0_millis;
unsigned char timer0_fract;

SIGNAL(TIMER0_OVF_vect)
{
    // copy these to local variables so they can be stored in registers
    // (volatile variables must be read from memory on every access)
    unsigned long m = timer0_millis;
    unsigned char f = timer0_fract;
 
    m += MILLIS_INC;
    f += FRACT_INC;
    if (f >= FRACT_MAX) {
        f -= FRACT_MAX;
        m += 1;
    }
 
    timer0_fract = f;
    timer0_millis = m;
    timer0_overflow_count++;
}
    
unsigned long millis()
{
    unsigned long m;
    uint8_t oldSREG = SREG;
 
    // disable interrupts while we read timer0_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer0_millis)
    cli();
    m = timer0_millis;
    SREG = oldSREG;
    sei();
    return m;
}

void timer_init(){
  // 64 prescaler for timer/counter0
  TCCR0 |= _BV(CS02);
  // Enable Timer/Counter0 Overflow Interrupt
  TIMSK |= _BV(TOIE0);
}
// __________________________________________________________
int led_status = 0;
void LED_on(char led){
  led_status |= _BV(led);
}
void LED_off(char led){
  led_status &= ~_BV(led);
}
void LED_all_off(){
  led_status = 0;
}
void LED_all_on(){
  led_status = 0xffff;
}
void LED_transmit(){
  char data = (led_status>>8 & 0xff);
  SPI_MasterTransmit(data);
  data = (led_status & 0xff);
  SPI_MasterTransmit(data);

  PORTE |= _BV(DDE5);
  _delay_ms(1);
  PORTE &= ~_BV(DDE5);
}

float angle(){
  if(timePerCycle==0){
    return 0;
  }
  float f = (millis()-timeCycle) / timePerCycle;
  return f;
}

void mode1(int seconds){
  float f = angle();
  char buffer[32];
  sprintf(buffer, "f=%3.2f timeCycle=%lu timePerCycle=%u\n", f, timeCycle, timePerCycle);
  USART_print(buffer);

  if(angle() < seconds/60.0){
    LED_on(0);
    LED_transmit();
  }
  else{
    LED_off(0);
    LED_transmit();
  }
}


// __________________________________________________________

ISR(INT0_vect)
{
  cli();
  USART_Transmit('i');
  USART_Transmit('\n');
  unsigned long t = millis();
  timePerCycle = t - timeCycle;
  timeCycle = t;

  /*char buffer[32];
  sprintf(buffer, "i timePerCycle=%u timeCycle=%lu\n", timePerCycle, timeCycle);
  USART_print(buffer);*/
  sei();
}

int main() {
    DDRE |= _BV(DDE4)|_BV(DDE5);
    PORTE &= ~_BV(DDE4);
    PORTE &= ~_BV(DDE5);

    USART_Init(MYUBRR);
    SPI_MasterInit();
    timer_init();

    char buffer[32];

    unsigned char data = 0b00000001;
    SPI_MasterTransmit(data);
    SPI_MasterTransmit(data);


    // Hall sensor : change pin 0 on bus D to an INPUT
    DDRD &= ~_BV(DDD0);

    // Set the interrupt mode to logical change for interrupt 0
    // in the external interrupt configuration register
    EICRA = (1 << ISC01);

    // Allow external interrupt 0
    EIMSK |= (1 << INT0);

    // Allow global interrupts
    sei();



    while(1){
      //USART_ReceiveString(buffer, 32);
      /*float f = 1.12;
      sprintf(buffer, "f=%1.2f\n", f);
      USART_print(buffer);*/

      mode1(30);

      /*int b = PIND & _BV(PIND0);
      if(!b){
        LED_on(1);
        LED_on(2);
        LED_on(3);
        LED_on(13);
        LED_on(14);
        LED_on(15);
        LED_transmit();
      }
      else{
        LED_all_off();
        LED_transmit();
      }*/


      //_delay_ms(1000);
    }
}
