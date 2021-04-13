/*
 * servo.c
 *
 * Created: 4/12/2021 8:52:20 PM
 * Author : ElmerOMejia
 */ 


#define F_CPU 8000000UL			/* Define CPU Frequency e.g. here its 8MHz */
#include <avr/io.h>				/* Include AVR std. library file */
#include <stdio.h>				/* Include std. library file */
#include <util/delay.h>			/* Include Delay header file */

void adc_init(void);
uint16_t read_adc(uint8_t channel);

int main(void){
	adc_init();							/* Initialize ADC */
	DDRB |= (1<<PB1);				/* Make OC1A pin as output */
	TCNT1 = 0;					/* Set timer1 count zero */
	ICR1 = 2499;				/* Set TOP count for timer1 in ICR1 register */
	
TCCR1A = (1<<WGM11)|(1<<COM1A1);
/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */

	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
	while(1)
	{
		OCR1A = 65 + (read_adc(0)/4.35);			/* Read ADC channel 0 and make count in between 65-300 i.e. 0.5-2.4 mSec duty cycle variation */
	}
}

void adc_init(void)
{
	/** Setup and enable ADC **/
	ADMUX = (0<<REFS1)|    // Reference Selection Bits
	(1<<REFS0)|    // AVcc - external cap at AREF
	(0<<ADLAR)|    // ADC Left Adjust Result
	(0<<MUX2)|     // Analog Channel Selection Bits
	(0<<MUX1)|     // ADC0 (PC0 PIN23)
	(0<<MUX0);
	ADCSRA = (1<<ADEN)|    // ADC ENable
	(0<<ADSC)|     // ADC Start Conversion
	(0<<ADATE)|    // ADC Auto Trigger Enable
	(0<<ADIF)|     // ADC Interrupt Flag
	(0<<ADIE)|     // ADC Interrupt Enable
	(1<<ADPS2)|    // ADC Prescaler Select Bits
	(0<<ADPS1)|
	(1<<ADPS0);
}

uint16_t read_adc(uint8_t channel){
		ADMUX &= 0xF0;                    //Clear the older channel that was read
		ADMUX |= channel;                //Defines the new ADC channel to be read
		ADCSRA |= (1<<ADSC);                //Starts a new conversion
		while(ADCSRA & (1<<ADSC));            //Wait until the conversion is done
		return ADCW;                 //Returns the ADC value of the chosen channel
}
