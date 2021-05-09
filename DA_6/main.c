/*
 * DA_6.c
 *
 * Created: 5/5/2021 11:07:44 PM
 * Author : ElmerOMejia
 */ 

#define F_CPU 16000000UL			/* Define CPU clock Frequency e.g. here its 8MHz */
#include <avr/io.h>					/* Include AVR std. library file */
#include <util/delay.h>				/* Include delay header file */
#include <inttypes.h>				/* Include integer type header file */
#include <stdlib.h>					/* Include standard library file */
#include <stdio.h>					/* Include standard library file */
#include <avr/interrupt.h>			/* Include avr interrupt header file */
#include <util/twi.h>

#include "i2cmaster.h"
#include "uart.h"					/* Include USART header file */
#include "ICM20948.h"
uint8_t ret;        // return value

uint16_t raw;             // raw sensor value
uint16_t ACC_Data[3], GRYRO_Data[3];          // raw values


void ICM20948_writereg(uint8_t reg, uint8_t val)
{
	i2c_start(ICM20948+I2C_WRITE);
	i2c_write(reg);  // go to register e.g. 106 user control
	i2c_write(val);  // set value e.g. to 0100 0000 FIFO enable
	i2c_stop();      // set stop condition = release bus
}

uint8_t ICM20948_readreg(uint8_t reg)
{
	i2c_start(ICM20948+I2C_WRITE);  // set device address and write mode
	i2c_write(reg);                     // ACCEL_XOUT
	i2c_stop();      // set stop condition = release bus
	_delay_ms(100);
	i2c_start(ICM20948+I2C_READ);    // set device address and read mode
	raw = i2c_readNak();                // read one intermediate byte
	i2c_stop();
	return raw;
}

uint16_t ICM20948_readreg16(uint8_t reg)
{
	i2c_start(ICM20948+I2C_WRITE);  // set device address and write mode
	i2c_write(reg);                     // ACCEL_XOUT
	i2c_start(ICM20948+I2C_READ);    // set device address and read mode
	raw = i2c_readNak();                // read one intermediate byte
	raw = (raw<<8) | i2c_readNak();     // read last byte
	i2c_stop();
	return raw;
}

void ICM20948_get_whom_am_I()
{
	uint8_t data[1];
	ICM20948_writereg(ICM20948_REG_PWR_MGMT_1, 0x80);
	_delay_ms(10);
	ICM20948_writereg(ICM20948_REG_PWR_MGMT_1, 0x01);
	_delay_ms(10);  // Wait for 200 ms.
	data[0]=ICM20948_readreg(ICM20948_REG_WHO_AM_I);
	if (data[0] != ICM20948_DEVICE_ID)
	 USART_SendString("Device Not Found\n");
	else
	 USART_SendString("Device Found\n");
}
void Init_ICM20948()
{
	i2c_init();     // init I2C interface
	USART_SendString("I2C Initialized\n");
	_delay_ms(2);  // Wait for 200 ms.
   	ICM20948_get_whom_am_I();
}

void ICM20948_config(void)
{
	ICM20948_writereg(ICM20948_REG_PWR_MGMT_1, 0x80); // reset device
	_delay_ms(10); // Wait for 10 ms.
	ICM20948_writereg(ICM20948_REG_PWR_MGMT_1, 0x01); // power on
	_delay_ms(10);  
	ICM20948_writereg(ICM20948_REG_BANK_SEL, 0x20);	// Bank 2 select
	_delay_ms(10);
	ICM20948_writereg(ICM20948_REG_GYRO_SMPLRT_DIV, 0x00); // gyro sample rate
	_delay_ms(10);
	ICM20948_writereg(ICM20948_REG_ACCEL_SMPLRT_DIV_1,0x00); // accel sample rate
	_delay_ms(10);
	ICM20948_writereg(ICM20948_REG_GYRO_CONFIG_1, 0x18); // gyro config
	_delay_ms(10);
	ICM20948_writereg(ICM20948_REG_ACCEL_CONFIG, 0x18); // accel config
	_delay_ms(10);
	ICM20948_writereg(ICM20948_REG_BANK_SEL, 0x00); // bank 0 select 
}

int main(void)
{
	char buffer[30], float_[5];
    
USART_Init(9600);	// usart initialized
	USART_SendString("UART Initialized\n");
//	sei();             // Enable global interrupts
	_delay_ms(150);

	Init_ICM20948();    // Sensor init
	ICM20948_config(); // sensor config
	_delay_ms(200);     // Wait for 200 ms.
	while (1)
	{


		// Read Accelerometer data
		ACC_Data[0]=ICM20948_readreg16(ICM20948_REG_ACCEL_XOUT_H_SH);
		ACC_Data[1]=ICM20948_readreg16(ICM20948_REG_ACCEL_YOUT_H_SH);
		ACC_Data[2]=ICM20948_readreg16(ICM20948_REG_ACCEL_ZOUT_H_SH);
		// Read gyro data
		GRYRO_Data[0]=ICM20948_readreg16(ICM20948_REG_GYRO_XOUT_H_SH);
		GRYRO_Data[1]=ICM20948_readreg16(ICM20948_REG_GYRO_YOUT_H_SH);
		GRYRO_Data[2]=ICM20948_readreg16(ICM20948_REG_GYRO_ZOUT_H_SH);
		
		// divide accelerometer by sensitivity
		float acc_x = ACC_Data[0]/ 2048.0;
		float acc_y = ACC_Data[1]/ 2048.0;
		float acc_z = ACC_Data[2]/ 2048.0;
		// divide gyro by sensitivity
		float gy_x = GRYRO_Data[0]/ 16.4;
		float gy_y = GRYRO_Data[1]/ 16.4;
		float gy_z = GRYRO_Data[2]/ 16.4;
		// print accelerometer x,y,z axis
		dtostrf( acc_x, 3, 2, float_ );
		sprintf(buffer," Acc_x = %s\t",float_);
		USART_SendString(buffer);
		
		dtostrf( acc_y, 3, 2, float_ );
		sprintf(buffer," Acc_y = %s\t",float_);
		USART_SendString(buffer);
		
		dtostrf( acc_z, 3, 2, float_ );
		sprintf(buffer," Acc_z = %s\n",float_);
		USART_SendString(buffer);
		// print gyro x,y,z axis
		dtostrf( gy_x, 3, 2, float_ );
		sprintf(buffer," gy_x = %s\t",float_);
		USART_SendString(buffer);
		
		dtostrf( gy_y, 3, 2, float_ );
		sprintf(buffer," g_y = %s\t",float_);
		USART_SendString(buffer);
		
		dtostrf( gy_z, 3, 2, float_ );
		sprintf(buffer," g_z = %s\n",float_);
		USART_SendString(buffer);
		_delay_ms(1000);	
		
	}
	
}
/*
 * uart.h, uart.c, i2cmaster.c, and i2cmaster.h, ICM20948.h
 * are not included to save space but were
 * taken from cpe301 github.
 * They are included in my github.
*/
