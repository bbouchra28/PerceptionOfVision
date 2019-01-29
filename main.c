#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#include "usart.h"
#include "led.h"
#include "text.h"

volatile unsigned long timePerCycle = 1000; // ms
volatile unsigned long timeCycle = 0; // ms
int led_status = 0;

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


float angle(){
  if(timePerCycle==0){
    return 0;
  }
  float f = (millis()-timeCycle) / (float)timePerCycle;
  return f;
}

void mode1(int hours, int minutes, int seconds){

  if(hours > 12)
    hours -= 12;
  float f = angle();
  led_status = 0;

  float val = 0.5 - (seconds/60.0);
  if( (f<=0.5 && (f>=val || val < 0)) || (f>0.5 && val<0 && f >= 1+val) ){
    led_status |= 0b1111000000000000;
  }
  else{
    led_status &= ~0b1111000000000000;
  }

  val = 0.5 - (minutes/60.0);
  if( (f<=0.5 && (f>=val || val < 0)) || (f>0.5 && val<0 && f >= 1+val) ){
    led_status |= 0b0000001111000000;
  }
  else{
    led_status &= ~0b0000001111000000;
  }

  val = 0.5 - (hours/12.0);
  if( (f<=0.5 && (f>=val || val < 0)) || (f>0.5 && val<0 && f >= 1+val) ){
    led_status |= 0b0000000000001111;
  }
  else{
    led_status &= ~0b0000000000001111;
  }


  LED_transmit(led_status);
}

int hours_d = 0;
int minutes_d = 0;
void mode2(int hours, int minutes, int seconds){
  if(hours > 12)
    hours -= 12;

  float f = angle();
  led_status = 0;

  float val = 0.5 - (minutes/60.0);
  val = (val>0) ? val : 1+val;
  if( f>=val && minutes_d < 50){
    led_status |= 0b0000001111111111;
    ++minutes_d;
  }
  else{
    led_status &= ~0b0000001111111111;
  }

  val = 0.5 - (hours/12.0);
  val = (val>0) ? val : 1+val;
  if( f>=val && hours_d < 50 ){
    led_status |= 0b0000000000001111;
    ++hours_d;
  }
  else{
    led_status &= ~0b0000000000001111;
  }

  LED_transmit(led_status);
}
// __________________________________________________________
int hours = 15;
int minutes = 30;
int seconds = 0;
unsigned long lastUpdateAt = 0;

void update_time(){
  unsigned long time = millis();
  int dSeconds = (time - lastUpdateAt) / 1000;
  seconds += dSeconds;
  if(dSeconds>0)
    lastUpdateAt = time;

  if(seconds >= 60){
    int deltaM = seconds / 60;
    seconds -= deltaM * 60;
    minutes += deltaM;

    if(minutes >= 60){
      int deltaH = minutes / 60;
      minutes -= deltaH * 60;
      hours += deltaH;

      if(hours >= 24){
        hours = hours % 24;
      }
    }
  }
}
// __________________________________________________________

ISR(INT0_vect)
{
  cli();

  unsigned long t = millis();

  int diff = (t - timeCycle - timePerCycle);
  int diff1 = t - timeCycle;

  timePerCycle = (timePerCycle*0.3 + diff1*0.7)+2; // prendre en compte la dernière valeur pour plus de précision
  if(timePerCycle<0)
    timePerCycle = 1;
  timeCycle = t;


  minutes_d = 0;
  hours_d = 0;
  update_time();


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

    // Hall sensor : change pin 0 on bus D to an INPUT
    DDRD &= ~_BV(DDD0);

    // Set the interrupt mode to logical change for interrupt 0
    // in the external interrupt configuration register
    EICRA = (1 << ISC01);

    // Allow external interrupt 0
    EIMSK |= (1 << INT0);

    // Allow global interrupts
    sei();
    void (*mode_function)(int, int, int);
    mode_function = &mode1;

    while(1){

      /*if(USART_Available()){
        int n = USART_ReceiveString((unsigned char*)buffer, 32);
        if(n>=2 && buffer[0]=='m' && buffer[1]>='1' && buffer[1]<='2'){
            if(buffer[1]=='2')
            {
              mode_function = &mode2;
              sprintf(buffer, "MODE 2");
              USART_print((unsigned char*)buffer);
            }
            else
            {
              mode_function = &mode1;
              sprintf(buffer, "MODE 1");
              USART_print((unsigned char*)buffer);
            }
        }
        else if(n>=9 && buffer[0]=='h'){
          int h = atoi(buffer+1);
          int m = atoi(buffer+4);
          int s = atoi(buffer+7);

          if(h<0 && h>=24)
            h = 0;
          if(m<0 && m>=60)
            m = 0;
          if(s<0 && s>=60)
            s = 0;

          hours = h;
          minutes = m;
          seconds = s;

          sprintf(buffer, "%d:%d:%d", hours, minutes, seconds);
          USART_print((unsigned char*)buffer);
        }
      }
      mode_function(hours, minutes, seconds);*/
      displayChar('A');
    }
}
