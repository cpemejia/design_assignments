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



volatile unsigned int newADC;

uint16_t read_adc(uint8_t channel);
void adc_init(void);


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


	adc_init();
	// Set timer0
	OCR0A = 45; // initial
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
			
			
			newADC = (read_adc(0)/4);			// 10 bit ADC to 8 bit
			_delay_ms(500);	
			if (newADC < 45){					// if MIN OCRA
				OCR0A = 45;
			}
			else if (newADC > 250){				// MAX OCRA
				OCR0A = 250;
			}
			else {
				OCR0A = newADC;
			}
					
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
	return ADCW;                    //Returns the ADC value of the chosen channel
}
