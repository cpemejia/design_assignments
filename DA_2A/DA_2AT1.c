/*
 * DA_2A.c
 *
 * Created: 2/23/2021 5:44:39 PM
 * Author : ElmerOMejia
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL 
#include <util/delay.h>

int main (void)
{
	DDRB |= (1<<PB3);		// set PB3 output
	PORTB |= (1<<PB3);      // set PB3 off
	//set PORTC.3 for input
	DDRC &= (0 << 3);
	PORTC |= (1 << 3);
	
	while(1){
		PORTB &= ~(1<<PB3); // output PB3 on
		_delay_ms(750);     // delay of 0.75s
		PORTB |= (1<<PB3); // output PB3 off
		_delay_ms(250);     // delay of 0.25s
	}
  
	return 1;
}
