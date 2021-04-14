/*
 * util.h
 *
 * Created: 21/10/2020 2:55:22 PM
 *  Author: s4581368
 */ 
#ifndef UTIL_H_
#define UTIL_H_

void request(void);
void response(void);
uint8_t receive_data(void);
void UART_Init(void);
void USART_Transmit(char data);
char USART_Receive (void);
void init_chip(void);

#endif /* UTIL_H_ */