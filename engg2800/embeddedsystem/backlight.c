#include <avr/io.h>

#define _BV(bit) (1 << (bit))
#define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))
#define loop_until_bit_is_set(sfr, bit) do { } while (bit_is_clear(sfr, bit))
#define loop_until_bit_is_clear(sfr, bit) do { } while (bit_is_set(sfr, bit))

// Global variables
uint16_t photoresistorValue;
uint16_t threshold_level[] = {140, 180, 280, 400, 500};
int comparator = 0;

/*
	Initialize ADC0 on 328p
*/
void init_ADC0(void) {
	ADMUX |= (1 << REFS0); //reference voltage on AVCC
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //ADC clock prescaler /8
	ADCSRA |= (1 << ADEN); //enables the ADC
}

/*
	Initialize and enable timer2 as PWM on 328p
*/
void timer2_setup(void) {
	DDRD |= (1 << 3) | (1 << 7) | (1 << 5);
	OCR2B = 0;
	
	// Set up timer/counter 0 for Fast PWM
	TCCR2A = (1<<COM2B1) | (1<<COM2B0) | (1<<WGM01) | (1<<WGM00);
	TCCR2B = (0<<WGM02) | (0<<CS22) | (0<<CS21) | (1<<CS20);
}

/*
	Reads the ADC and updates the LCD back light depending on ambient light level
*/
void update_lcd_backlight(void) {
	ADCSRA |= (1 << ADSC); //start ADC conversion
	loop_until_bit_is_clear(ADCSRA, ADSC); //wait until ADC conversion is done
	photoresistorValue= ADC; //read ADC value in
	if (photoresistorValue <= threshold_level[0]) {
		// Brightness setting 1
		comparator = 254;
	}
	if (photoresistorValue > threshold_level[0] && photoresistorValue < threshold_level[1]) {
		// Brightness setting 2
		comparator = 230;
	}
	if (photoresistorValue >= threshold_level[1] && photoresistorValue < threshold_level[2]) {
		// Brightness setting 3
		comparator = 180;
	}
	if (photoresistorValue >= threshold_level[2] && photoresistorValue < threshold_level[3]) {
		// Brightness setting 4
		comparator = 140;
	}
	if (photoresistorValue >= threshold_level[3] && photoresistorValue < threshold_level[4]) {
		// Brightness setting 5
		comparator = 100;
	}
	if (photoresistorValue >= threshold_level[4]) {
		// Brightness setting 6
		comparator = 0;
	}
	if (OCR2B > comparator) {
		OCR2B -= 2;
	}
	if (OCR2B < comparator) {
		OCR2B += 2;
	}
}
