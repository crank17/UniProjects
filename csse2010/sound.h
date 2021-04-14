/*
 * sound.h
 *
 * Created: 11/06/2020 12:51:20 AM
 *  Author: Ryan
 */ 


#ifndef SOUND_H_
#define SOUND_H_

uint16_t freq_to_clock_period(uint16_t freq);
uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod);
void init_sound(void);
void set_sound(uint8_t val);
void play_sound(void);





#endif /* SOUND_H_ */