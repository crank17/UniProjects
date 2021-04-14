/*
 * IncFile1.h
 *
 * Created: 27/05/2020 8:13:35 PM
 *  Author: Ryan
 */ 


#ifndef GAME_H_
#define GAME_H_
#include <stdint.h>
void update_level(void);
uint32_t get_level(void);
void level_up(void);
void display_level(void);
void init_level(void);
void display_lvl_message(void);
void check_level(void);
void reset_counter(void);
uint8_t get_counter(void);
void init_lives(void);
void lose_life(void);
void display_lives_led(void);
uint32_t get_lives(void);
uint8_t lives_flag(void);
uint8_t get_speed(void);
void set_speed(void);
void init_speed(void);
uint8_t get_button_state(void);





#endif /* INCFILE1_H_ */