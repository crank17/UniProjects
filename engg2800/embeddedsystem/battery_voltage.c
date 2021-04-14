/*
 * battery_voltage.c
 *
 * Created: 25/10/2020 2:43:22 PM
 *  Author: Komei Jo
 */ 
#include <avr/io.h>

#define _BV(bit) (1 << (bit))
#define bit_is_set(sfr, bit) (_SFR_BYTE(sfr) & _BV(bit))
#define bit_is_clear(sfr, bit) (!(_SFR_BYTE(sfr) & _BV(bit)))
#define loop_until_bit_is_set(sfr, bit) do { } while (bit_is_clear(sfr, bit))
#define loop_until_bit_is_clear(sfr, bit) do { } while (bit_is_set(sfr, bit))

uint16_t voltage;

/*
 *	Initialize the ADC1
 */
void init_ADC1(void) {
	ADMUX |= (1 << REFS0) | (1 << MUX0); //reference voltage on AVCC and read from ADC1
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0); //ADC clock prescaler /8
	ADCSRA |= (1 << ADEN); //enables the ADC
}

/*
 *	Reads the ADC and returns the battery voltage
 */
uint16_t update_battery_voltage(void) {
	ADCSRA |= (1 << ADSC); //start ADC conversion
	loop_until_bit_is_clear(ADCSRA, ADSC); //wait until ADC conversion is done
	return ADC; //read ADC value in
	
}
