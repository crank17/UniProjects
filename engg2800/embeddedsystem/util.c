/*
 * SerialTesting.c
 *
 * Created: 12/09/2020 11:01:12 AM
 * Author : Ryan
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdarg.h>
#include "accel.h"
#include "i2cmaster.h"
#include "util.h"

#define DHT11_PIN 6
uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;
uint8_t ampm = 0, counter = 0;
int i = 0;
volatile uint8_t togglespeed = 0;

/* Microcontroller send start pulse/request */
void request(void) {
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN); /* set to low pin */
	_delay_ms(20); /* wait for 20ms */
	PORTD |= (1<<DHT11_PIN); /* set to high pin */
}

/* receive response from DHT11 */
void response(void) {
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND& (1<<DHT11_PIN));
}

/* receive data */
uint8_t receive_data(void) {
	for (int q=0; q<8; q++) {
		while((PIND & (1<<DHT11_PIN)) == 0);  /* check received bit 0 or 1 */
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN))/* if high pulse is greater than 30ms */
		c = (c<<1)|(0x01); /* then its logic HIGH */
		else /* otherwise its logic LOW */
		c = (c<<1);
		while(PIND & (1<<DHT11_PIN));
	}
	return c;
}

//Initialize UART
void UART_Init(void) {
	UBRR0H = (51>> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
	UBRR0L = 51;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1 << RXCIE0);
	UCSR0C =  (1<<USBS0)|(1<<UCSZ01);
}

//Start transmitting
void USART_Transmit(char data) {
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

//Receive transmitting
char USART_Receive (void) {
	while(!((UCSR0A) & (1<<RXC0)));                   // wait while data is being received
	return UDR0;                                      // return 8-bit data
}

//Initialise the timer registers.
void init_chip(void) {
	// TIMER 0 for interrupt frequency 1000 Hz:
	DDRC &= ~(1 << PINC3);
	DDRB = (1 << 1);
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM12)|(1 << CS10);
	
	cli(); // stop interrupts
	TCCR0A = 0; // set entire TCCR0A register to 0
	TCCR0B = 0; // same for TCCR0B
	TCNT0  = 0; // initialize counter value to 0
	
	// set compare match register for 1000 Hz increments
	OCR0A = 124; // = 8000000 / (64 * 1000) - 1 (must be <256)
	// turn on CTC mode
	TCCR0B |= (1 << WGM01);
	// Set CS02, CS01 and CS00 bits for 64 prescaler
	TCCR0B |= (0 << CS02) | (1 << CS01) | (1 << CS00);
	// enable timer compare interrupt
	TIMSK0 |= (1 << OCIE0A);
	sei(); // allow interrupts
	
	//Setup
	UART_Init();
}


