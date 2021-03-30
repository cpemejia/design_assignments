/*
 * Mid1.c
 *
 * Created: 3/28/2021 8:00:23 PM
 * Author : ElmerOMejia
 */ 


#define F_CPU 16000000UL				// 16M w/ 9600 baud rate
#define BAUD 9600
#define BAUD_PRESCALE (((F_CPU / (BAUD * 16UL))) - 1)


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <string.h>

// declaration of variables
volatile unsigned char in_data[8];
volatile unsigned char in_cmd[8];
volatile unsigned char count;
volatile unsigned char command;

uint16_t adc_value;            //Variable used to store the value read from the ADC
char buffer[5];                //Output of the itoa function

// Variables to hold current settings
unsigned int freq = 0;
float duty_cycle;

void usart_putc (char send)
{
	// send data
	while ((UCSR0A & (1 << UDRE0)) == 0) {};
	UDR0 = send;
}

void usart_puts (const char *send)
{
	// send each character in string
	while (*send) {
		usart_putc(*send++);
	}
}

void usart_ok()
{
	usart_puts("OK\r\n"); // OK confirm
}

void copy_command ()
{
	ATOMIC_BLOCK(ATOMIC_FORCEON) { // copy content and use atomic so no info lost
		memcpy(in_cmd, in_data, 8);
		memset(in_data[0], 0, 8);
	}
}

unsigned long parse_assignment ()
{
	char *pch;
	char cmdValue[16];
	// search for equal sign
	pch = strchr(in_cmd, '=');
	// when found copy the following chars
	strcpy(cmdValue, pch+1);
	// convert to int and return 
	return atoi(cmdValue);
}

void init_timer1(unsigned int freq)
{
	DDRB |= (1<<PB2);
	TCNT1 = 0;
	ICR1 = freq;
	OCR1B = 5000;
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	// Mode 12, CTC on ICR1
	TIMSK1 |= (1 << OCIE1B) | (1 << ICIE1);
	//Set interrupt on compare match
	TCCR1B |= (1 << CS12);
	// set prescaler to 256 and starts the timer
}

void init_timer3(unsigned int dc)
{
	DDRB |= (1 << PB3); // PB3 is an output
	TCNT3 = 0;
	ICR3 = 50000;
	OCR3B = dc*ICR3;
	TCCR3B |= (1 << WGM32) | (1 << WGM33);
	// Mode 12, CTC on ICR3
	TIMSK3 |= (1 << OCIE3B) | (1 << ICIE3);
	//Set interrupt on compare match
	TCCR3B |= (1 << CS32);
	// set prescaler to 256 and starts the timer
}
//f_out = 16M/(2*64*63) =

void adc_init(void){
	ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));    //16Mhz/128 = 125Khz the ADC reference clock
	ADMUX |= (1<<REFS0);                //Voltage reference from Avcc (5v)
	ADCSRA |= (1<<ADEN);                //Turn on ADC
	ADCSRA |= (1<<ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running
}

uint16_t read_adc(uint8_t channel){
	ADMUX &= 0xF0;                    //Clear the older channel that was read
	ADMUX |= channel;                //Defines the new ADC channel to be read
	ADCSRA |= (1<<ADSC);                //Starts a new conversion
	while(ADCSRA & (1<<ADSC));            //Wait until the conversion is done
	ADCSRA|=(1<<ADIF);
	return ADCW;                    //Returns the ADC value of the chosen channel
}


void process_command()
{
	usart_puts("Help Menu:\n");
	usart_puts("h - help screen\n");
	usart_puts("t - T in C of LM34/35\n");
	usart_puts("f - T in F of LM34/35\n");
	usart_puts("o - ON LED @PB5\n");
	usart_puts("O - OFF LED @PB5\n");
	usart_puts("d - DC for LED @PB3\n");
	usart_puts("i - Frequency for LED @PB2\n");
	usart_puts("'=' after d or i for change\n");
	usart_puts("0<d<100\n");
	usart_puts("5k<i<65k\n");
	usart_puts("\n");
	switch (in_cmd[0]) {
		case 'h':										// h for help menu
		usart_puts("Help Menu:\n");
		usart_puts("h - help screen\n");
		usart_puts("t - T in C of LM34/35\n");
		usart_puts("f - T in F of LM34/35\n");
		usart_puts("o - ON LED @PB5\n");
		usart_puts("O - OFF LED @PB5\n");
		usart_puts("d - DC for LED @PB3\n");
		usart_puts("i - Frequency for LED @PB2\n");
		break;
		case 't':
		usart_puts("Reading channel ");		// read temp on adc
		usart_puts(" : \n");
		adc_value = read_adc(0);
		itoa(adc_value, buffer, 10);
		usart_puts(buffer);
		usart_puts(" \n ");
		break;
		case 'f':
		usart_puts("Reading channel ");		// read temp on adc convert to F
		usart_puts(" : \n");
		adc_value = read_adc(0);
		adc_value = ((adc_value * 9) / 5) + 32;
		itoa(adc_value, buffer, 10);
		usart_puts(buffer);
		usart_puts(" \n ");
		break;
		case 'o':
		PORTB &= ~(1<<PB5); // output PB5 on
		break;
		case 'O':
		PORTB |= (1<<PB5); // output PB5 off
		break;
		case 'd':
		if (in_cmd[1] == '=') {
			// Do the query action for S
			duty_cycle = parse_assignment();
			init_timer3(duty_cycle);
		}
		else{
			init_timer3(50);
		}
		break;
		case 'i':
		if (in_cmd[1] == '=') {
			// Do the query action for S
			freq = parse_assignment();
			init_timer1(freq);
		}
		else{
			init_timer1(10000);
		}
		break;
		default:
		usart_puts("INCORRECT 'h' for list\r\n");
		break;
	}
}

void UART0_init(void)
{
	// Turn on USART hardware (RX, TX)
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	// 8 bit char sizes
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);
	// Set baud rate
	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_PRESCALE;
	// Enable the USART Receive interrupt
	UCSR0B |= (1 << RXCIE0 );
}

void out_init(void)
{
	DDRB |= (1<<PB5);		// set PB5 output
	DDRC |= (1<<PC0);        // set PC0 as output
}

int main(void)
{
	UART0_init();
	adc_init();
	out_init();
	// Globally enable interrupts
	sei();

	while(1) {

		if (command == 1) {
			copy_command();		// copy command
			process_command();  // process the command 

			command = 0;
			usart_ok();			// OK for confirmation
		}

	}
}


ISR (USART0_RX_vect)
{
	// get data from uart
	in_data[count] = UDR0;

	if (in_data[count] == '\n') {
		command = 1;
		// reset count to 0 at end of line
		count = 0;
		} else {
		count++;
	}
}

ISR (TIMER1_COMPB_vect)
{
	PORTB &= ~(1<<PB2);				// PB2 on on comp B match
}

ISR (TIMER1_CAPT_vect)
{
	PORTB |= (1<<PB2);				// PB2 off on Capture
	TCNT1 = 0;
}

ISR (TIMER3_COMPB_vect)
{
	PORTB &= ~(1<<PB3);				// PB3 on on comp B match
}

ISR (TIMER3_CAPT_vect)
{
	PORTB |= (1<<PB3);				// PB3 off on Capture
	TCNT3 = 0;
}

