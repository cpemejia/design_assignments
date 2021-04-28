/*
 * MID2.c
 *
 * Created: 4/17/2021 2:19:23 PM
 * Author : ElmerOMejia
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#define AIN2 5
#define AIN1 4

// PD6 PWMA
// PD5 AIN2
// PD4 AIN1
// ADC0/PC0
   
int main(void)
{
	uint8_t flag = 0;
	//speed PWMA
	DDRD |= (1<<6); 		// set PD6 output
	// direction AIN2/AIN1
	DDRD |= (1<<5);		// set PD5 output
	DDRD |= (1<<4);		// set PD4 output
	PORTD &= ~(1<<AIN2);
	PORTD &= ~(1<<AIN1);


	// Set timer0
	OCR0A = 250; // initial
	// Fast PWM, non inverted
	TCCR0A |= ((1<<COM0A1) | (1<<WGM01) | (1<<WGM00));
	TCCR0B |= (1<<CS01); // n = 8;


	while (1)
	{
			if (flag == 1)
			{
			_delay_ms(500);	
			PORTD |= (1<<AIN1);   // H
			PORTD &= (~(1<<AIN2));  // L
		    _delay_ms(5000);
			flag = 0;
			
			}
			else{
			PORTD &= (~(1<<AIN1));  // L
			PORTD |= (1<<AIN2);  // H
		    _delay_ms(5000);
			flag = 1;
			}
	}
}
