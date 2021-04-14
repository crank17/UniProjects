/*
 * joystick.h
 *
 * Created: 8/06/2020 8:35:08 PM
 *  Author: Ryan
 */ 


#ifndef JOYSTICK_H_
#define JOYSTICK_H_
#include <stdint.h>

uint8_t get_joystick_val(void);
uint8_t joystick_input(void);
void InitADC(void);


#endif /* JOYSTICK_H_ */