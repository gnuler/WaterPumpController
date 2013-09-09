#ifndef common_h
#define common_h

#include <avr/interrupt.h>

//extern "C" void __cxa_pure_virtual() { while (1); }


#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)




volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;


void initTimer0(void)
{

    /*
    >>> freq=8000000
    >>> preescaler = 64.0
    >>> resolution = preescaler/freq
    >>> resolution
    8e-06
    >>> timer_value = 0.001/resolution
    >>> timer_value
    125.00000000000001
    */


    //Enable Timer0 overflow Interrupt
    TIMSK0|=(1<<TOIE0);

    //Set Initial Timer value
    // 255-125 = 130
    TCNT0=130;

    TCCR0B|=(1<<CS01)|(1<<CS00);
    //TCCR0B|=(1<<CS01)|(1<<CS00);
    //Enable global interrupts

}




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

   TCNT0=130;
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

   return m;
}



#endif
