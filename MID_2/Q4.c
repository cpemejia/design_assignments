/*
 * Q4.c
 *
 * Created: 4/25/2021 6:59:04 PM
 * Author : ElmerOMejia
 */ 

#define BAUD 9600
#define F_CPU 1000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
// PID
#define P_val 1.0
#define I_val 0.0
#define D_val 0.0

#define AIN2 5
#define AIN1 4
#define I_memory 20

// PD6 PWMA
// PD5 AIN2
// PD4 AIN1
// ADC0/PC0

// global variables for PID
	volatile int32_t mean_I_err[I_memory],
					 PID=0,
					 err=0,
					 err_old=0,
					 mean_I_error=0,
					 I_err=0.0,
					 P_err=0,
					 D_err=0;
	volatile float	 set_speed = 100.0,
					 current_speed = 0.0;
	volatile uint16_t duty = 0,				// initial 
					  duty_max = 250;		// about 100 RPM
	volatile uint8_t flag = 0;
	
// global variables for RPM
volatile uint32_t T1Ovs1, T1Ovs2;
volatile uint32_t revTick1, revTick2, revTick3, revTick4;
volatile uint8_t Flag;
volatile uint32_t period;
volatile uint16_t revCtr;
volatile float RPM;

float calc_spd_1x(uint32_t p);// func to calc speed
void pid_controller(void);
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
	
	for(int i=0;i<I_memory-1;i++){
		mean_I_err[i]=(int32_t)0.0;
		}
		
	init_timer0_pwm();
	init_timer2();
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
			current_speed = calc_spd_1x(period);
			snprintf(outs, sizeof(outs),"%f rpm", calc_spd_1x(period));
			USART_putstring(outs);
			USART_putstring("\r\n");
			_delay_ms(5000);
			//clear variables and registers
			Flag=0;
			T1Ovs2=0;
			TIFR1=(1<<ICF1)|(1<<TOV1);
			TIMSK1|=(1<<ICIE1)|(1<<TOIE1);
		}
		
					if (flag == 1){
						pid_controller();
						OCR0A = (duty/256);
						//	snprintf(outs, sizeof(outs),"%f rpm", set_speed);
						//	USART_putstring(outs);
					}
	}
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
//	//stop input capture and overflow interrupts
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

ISR(TIMER2_COMPA_vect){
				if (flag == 1)
				{
					flag = 0;
					
				}
				else{
					flag = 1;
				}	
}


void init_timer0_pwm(void){
	DDRD |= (1<<DDD6); // PD6 out
	TCNT0 = 0;
	OCR0A = 250; //  duty
	TCCR0A |= (1<<COM0A1); // non-inverting
	TCCR0A |= (1<<WGM01) | (1<<WGM00); // fast pwm mode
	TCCR0B |= (1<< CS02); // prescaler 256
}
void init_timer2(void){
	TCCR2A |= (1 << WGM21)| (1<<COM2A0);//Set TCCR2A to CTC and toggle compare match
	OCR2A = 194;                         // Set OCR2A for 10hz
	TCNT2 = 0;                          // begin counter at 0
	TCCR2B |= (1<<CS22)|(1<<CS21);                // set prescaler to 256
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

void pid_controller (void){
	
	// Make sure that this is executed @ 10Hz (dt=0.1) 
	// current_speedin rpm is updated by the encoder
	//PID
	err_old=err;
	err=set_speed-current_speed;
	P_err=err;
	
	//Proportional is directly the difference times P_val
	//save new value to last
	mean_I_err[I_memory-1]=(int32_t)err;
	mean_I_error=0;
	//move all samples left by one in array and sum them up
	for(int i=0;i<I_memory-1;i++){
		mean_I_err[i]=(int32_t)mean_I_err[i+1];
		mean_I_error+=(int32_t)mean_I_err[i];
		}
	
	mean_I_error+=(int32_t)err; //take the latest value in account
	I_err=(int32_t)mean_I_error;
	D_err=(int32_t)err-err_old;
	PID=(int32_t)P_val*P_err+I_val*I_err*0.1+D_val*D_err/0.1;
	if(PID>duty_max){
		PID=duty_max;
	}
	else if(PID<0){
		PID=0;
	}
	duty=(uint16_t)PID;
}


void USART_init( void )
{
	UBRR0H = 0;
	UBRR0L = F_CPU/1/BAUD - 1; // Used for the BAUD prescaler
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
