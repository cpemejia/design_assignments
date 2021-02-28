/*
 * DA_2BC.c
 *
 * Created: 2/27/2021 11:57:51 PM
 * Author : ElmerOMejia
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>


int main(void)
{
	DDRB |= (1<<PB3);		// set PB3 output
	PORTB |= (1<<PB3);      // set PB3 off
	
	PORTD = 1<<2; // pull up enable
	EICRA = 0x02; 
	EIMSK = (1<<INT0); // enable interrupt 0
	sei(); // enable interrupts

    while (1) 
    {
		
		PORTB &= ~(1<<PB3); // output PB3 on
		_delay_ms(750);     // delay of 0.75s
		PORTB |= (1<<PB3); // output PB3 off
		_delay_ms(250);     // delay of 0.25s
    }
}


ISR (INT0_vect) // ISR for INT0
{
		    PORTB |= (1<<PB3); // output PB3 off
			DDRB |= (1<<PB2);		// set PB2 output
			PORTB &= ~(1<<PB2); // output PB2 on
			_delay_ms(1750);  // delay of 2s
			PORTB |= (1<<2);  // PB2 off
}

