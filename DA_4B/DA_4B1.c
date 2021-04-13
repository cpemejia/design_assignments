/*
 * DA_4B.c
 *
 * Created: 4/10/2021 8:21:16 PM
 * Author : ElmerOMejia
 */ 

#define F_CPU 16000000UL/* Define CPU Frequency 1MHz */
#include <avr/io.h> /* Include AVR std. library file */

volatile unsigned int period;

uint16_t read_adc(uint8_t channel);
void adc_init(void);
void delay(int period);

int main(void)
{
   /* Make PORTD lower pins as output */
	DDRD = 0x0F;

	// Set the Timer Mode to CTC 
	TCCR0A |= (1 << WGM01);
	// Set the value that you want to count to
	period = 30;
	// initialize counter
	TCNT0 = 0;
	// set prescaler to 1024 and start the timer
	TCCR0B |= (1 << CS02)|(1<<CS00);
    

	adc_init();
	
    while (1) 
    {
		if (period < 30)          			// if MIN OCRA
			period = 30;
		else if (period > 230)
			period = 230; 
		else 
			period = (read_adc(0)/4);			// 10 bit ADC to 8 bit
			
		/* Rotate Stepper Motor Anticlockwise with Full step sequence */
		PORTD=0x09;
		delay(period);
		PORTD=0x03;
		delay(period);
		PORTD=0x06;
		delay(period);
		PORTD=0x0C;
		delay(period);

	}
		
}

void delay(int period){
	OCR0A = period;
	while( (TIFR0 & (1<<OCF0A)) ==0 );
	TCNT0 = 0;
	TIFR0 |=(1<<OCF0A);
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
