/*
 * q3.c
 *
 * Created: 4/24/2021 4:58:46 PM
 * Author : ElmerOMejia
 */ 

#define BAUD 9600
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#define AIN2 5
#define AIN1 4

// PD6 PWMA
// PD5 AIN2
// PD4 AIN1
// ADC0/PC0
volatile uint32_t T1Ovs1, T1Ovs2;
volatile uint32_t revTick1, revTick2, revTick3, revTick4;
volatile uint8_t Flag = 0;
uint32_t period;
uint16_t revCtr;

volatile float RPM;

float calc_spd_1x(uint32_t p);
void USART_send(char data); // Used to send integer to terminal
void USART_putstring(char* StringPtr); // Used to take in every character in the string and sends it to the terminal
void USART_init(void); // Initializes the analog to digital functions, as well as OVF interrupt

char outs[20];
int main(void)
{
	
	//speed PWMA
	DDRD |= (1<<6); 		// set PD6 output
	// direction AIN2/AIN1
	DDRD |= (1<<5);		// set PD5 output
	DDRD |= (1<<4);		// set PD4 output
	DDRB &= ~(1<<0);     // set SIG input
	PORTB |= (1<<0);
	PORTD &= ~(1<<AIN2);
	PORTD &= ~(1<<AIN1);
	
	init_timer0_pwm();
	USART_init();
	TCNT1 = 0;							// counter = 0
	TIMSK1 |= (1<<ICIE1) | (1<<TOIE1);  // enable ICP1&ovf interrupts
	TCCR1B |= (1<<ICES1) | (1<<CS10);	// rsing edge, prescaler 1
	sei();
	
	while (1)
	{
		_delay_ms(500);
		PORTD |= (1<<AIN1);   // H
		PORTD &= (~(1<<AIN2));  // L
	
		if (Flag==3)
		{
			period=((uint32_t)(revTick3-revTick1)+((uint32_t)T1Ovs2*0x10000L));
		//	period=((uint32_t)(((revTick3-revTick1)+(revTick4-revTick2))/2)+((uint32_t)T1Ovs2*0x10000L));
			snprintf(outs, sizeof(outs),"%f rpm", calc_spd_1x(period));
			USART_putstring(outs);
			USART_putstring("\r\n");
			_delay_ms(500);
			//clear variables and registers
			Flag=0;
			T1Ovs2=0;
			TIFR1=(1<<ICF1)|(1<<TOV1);
			TIMSK1|=(1<<ICIE1)|(1<<TOIE1);
		}
	}
}

float calc_spd_1x(uint32_t p)
{
	// average of 10 samples
	float rpm1 = 0;
	for (int i = 0; i<10; i++){
	rpm1 = rpm1 + ((p*60)/(96))/50;
	}
	return rpm1/10;			// return rpm
}

ISR(TIMER1_CAPT_vect){
if (Flag==0)
{
	//save captured timestamp
	revTick1=ICR1;
	//change capture on falling edge
	TCCR1B&=~(1<<ICES1);
	//reset overflows
	T1Ovs2=0;
}
if (Flag==1)
{
	revTick2=ICR1;
	//change capture on rising edge
	TCCR1B|=(1<<ICES1);
}
if (Flag==2)
{
	revTick3=ICR1;
	TIMSK1&=~((1<<ICIE1)|(1<<TOIE1));
	//change capture on falling edge
	//TCCR1B&=~(1<<ICES1);
}
/*
if (Flag == 3)
{
	revTick4 = ICR1;
	TIMSK1&=~((1<<ICIE1)|(1<<TOIE1));
}
*/
//increment Flag
Flag++;
}


ISR(TIMER1_OVF_vect){
	//increment overflow counter
	T1Ovs2++;
}

void init_timer0_pwm(void){
	DDRD |= (1<<DDD6); // PD6 out
	TCNT0 = 0;
	OCR0A = 250; // 50% duty
	TCCR0A |= (1<<COM0A1); // non-inverting
	TCCR0A |= (1<<WGM01) | (1<<WGM00); // fast pwm mode
	TCCR0B |= (1<< CS02); // prescaler 256
}

void USART_init( void )
{
	UBRR0H = 0;
	UBRR0L = F_CPU/16/BAUD - 1; // Used for the BAUD prescaler
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

void USART_send(char data)
{
	while (!(UCSR0A & (1 << UDRE0))); // Until UDRE0 goes high, it will keep looping
	UDR0 = data; // UDR0 register grabs the value given from the parameter
	
}

void USART_putstring(char *StringPtr)
{
	while ((*StringPtr != '\0')){ // Until it reaches the end of the line, it will keep looping
		while (!(UCSR0A & (1 << UDRE0))); // Until UDRE0 goes high, it will keep looping
		UDR0 = *StringPtr; // UDR0 register grabs the value given from the parameter
		StringPtr++; // but it does it by every character as shown here
	}
}
