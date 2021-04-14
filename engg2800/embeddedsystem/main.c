/*
 * main.c
 *
 * Created: 25/10/2020 7:09:22 PM
 *  Author: Group07
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdarg.h>
#include <avr/io.h>
#include <u8g2.h>
#include <avr/eeprom.h>
#include "u8x8_avr.h"
#include "accel.h"
#include "i2cmaster.h"
#include "util.h"
#include "backlight.h"
#include "battery_voltage.h"
#include "LCD_funcs.h"

#define P_CPU_NS (1000000000UL / F_CPU)

#define DISPLAY_CLK_DIR DDRB
#define DISPLAY_CLK_PORT PORTB
#define DISPLAY_CLK_PIN 5

#define DISPLAY_DATA_DIR DDRB
#define DISPLAY_DATA_PORT PORTB
#define DISPLAY_DATA_PIN 3

#define DISPLAY_CS_DIR DDRB
#define DISPLAY_CS_PORT PORTB
#define DISPLAY_CS_PIN 2

#define DISPLAY_DC_DIR DDRB
#define DISPLAY_DC_PORT PORTB
#define DISPLAY_DC_PIN 6

#define DISPLAY_RESET_DIR DDRB
#define DISPLAY_RESET_PORT PORTB
#define DISPLAY_RESET_PIN 0

#define loop_until_bit_is_clear(sfr, bit) do { } while (bit_is_set(sfr, bit)) 
	
// Global variables
volatile uint32_t millis = 0;
uint16_t alarmarr[5];
uint8_t alarms[8];
int period[] = {1000, 2000, 3000, 4000};
int freqflag = 0;
int t = 0, stopFlag = 0;
int freqcount = 0;
volatile uint8_t timearr[3] = {00,00,00};
volatile uint8_t stoparr[4] = {00,00,00,0};
uint8_t humidityDummies[5] = {62.5, 64.3, 63.6, 59.7, 60.1};
uint8_t humidityDecs[5] = {5, 3, 6, 7, 1};
uint8_t tempDummies[5] = {23, 24, 22, 23, 25};
uint8_t tempDecs[5] = {8, 5, 3, 2, 0};
uint8_t accelDummies[5] = {1, 1, 0, 1, 0};
uint8_t on = 0;
volatile uint8_t AlarmTime[12] = {00,00,00,00,00,00,00,00,00,00,00,00,};
uint8_t uartcounter = 0;

volatile uint32_t secs = 0;
int periodcount = 0;
u8g2_t u8g2;
char night[] = "PM";
char day[] = "AM";
int past_mode, mode;
char str[20];
int past_mode = 0 ;
uint16_t voltageValue;
float floatVolts;
int tapStatus;
int timerSet = 0;
int timerFlag = 0, alarmFlag = 0;
uint16_t EEMEM tone1_f;
uint16_t EEMEM tone1_t;
uint16_t EEMEM tone2_f;
uint16_t EEMEM tone2_t;
uint16_t EEMEM tone3_f;
uint16_t EEMEM tone3_t;
uint16_t EEMEM tone4_f;
uint16_t EEMEM tone4_t;
uint16_t EEMEM alarm;

/*
 * Receives the data to set thencies in hertz. 
 */
void getAlarmFreq(){
	for (int i = 0; i < 8; i++) {
		alarms[i] = USART_Receive();
	}
	alarmarr[0] = alarms[1] | alarms[0] << 8;
	alarmarr[1] = alarms[3] | alarms[2] << 8;
	alarmarr[2] = alarms[5] | alarms[4] << 8;
	alarmarr[3] = alarms[7] | alarms[6] << 8;
	on = 1;
	
}

/*
 * Receives time from the UART. 
 */
void testfunc(){
	timearr[0] = USART_Receive();
	timearr[1] = USART_Receive();
	timearr[2] = USART_Receive();
}

/*
 * Checks if pin D7 is toggled. 
 */
uint8_t check_toggle() {
	uint8_t toggle =  (PIND & (1 << PIND7)) >> PIND7;
	return toggle;
}

/*
 * Checks if pin D5 is toggled. 
 */
uint8_t check_toggle_2() {
	uint8_t toggle =  (PIND & (1 << PIND5)) >> PIND5;
	return toggle;
}

void setAlarmTime(int i) {
	if (i == 1) {
		AlarmTime[0] = USART_Receive();
		AlarmTime[1] = USART_Receive();
		AlarmTime[2] = USART_Receive();
	}
	if (i == 2) {
		AlarmTime[3] = USART_Receive();
		AlarmTime[4] = USART_Receive();
		AlarmTime[5] = USART_Receive();
	}
	if (i == 3) {
		AlarmTime[6] = USART_Receive();
		AlarmTime[7] = USART_Receive();
		AlarmTime[8] = USART_Receive();
	}
	if (i == 4) {
		AlarmTime[9] = USART_Receive();
		AlarmTime[10] = USART_Receive();
		AlarmTime[11] = USART_Receive();
	}
}


/*
 * Interrupt Routine.
 */
ISR(TIMER0_COMPA_vect){	
	
	if (millis%490 == 0 && check_toggle() == 0){
		timearr[2]++;
	}
	if (millis%20 == 0) {
		init_ADC0();
		update_lcd_backlight();
	}

	if (millis%50 == 0 && stopFlag == 1){
		stoparr[3]++;
	}
	
	if (millis % 17 == 0 && check_toggle() == 1) {
		timearr[2]++;
	}
	if (timearr[2] >= 60){
		timearr[2] = 0;
		timearr[1]++;
	}
	if (timearr[1] == 60){
		timearr[1] = 0;
		timearr[0]++;
	}
	if (timearr[0] == 24){
		timearr[0] = 0;
	}
	
	if (stoparr[3] >= 9){
		stoparr[3] = 0;
		stoparr[2]++;
	}
	if (stoparr[2] >= 60){
		stoparr[2] = 0;
		stoparr[1]++;
	}
	if (stoparr[1] >= 60){
		stoparr[1] = 0;
		stoparr[0]++;
	}
	if (stoparr[0] == 24){
		stoparr[0] =0;
	}
	millis++;
	if (millis%600 == 0){
		USART_Transmit(mode + '0');
		USART_Transmit(accelDummies[uartcounter] + '0');
		USART_Transmit(tempDummies[uartcounter] + '0');
		USART_Transmit(humidityDummies[uartcounter] + '0');
		USART_Transmit(humidityDecs[uartcounter] + '0');
		uartcounter++;
		if(uartcounter > 4) {
			uartcounter = 0;
		}
	}
	
	
	
	
	if (check_toggle_2() == 1 || alarmFlag == 0){
		OCR1A = 0;
		t = 0;
		return;
	}
	if (alarmFlag == 1){
		if (t >= 0 && t < period[0]){
			if (alarmarr[0] == 0){
				OCR1A = 0;
			} else {
				OCR1A = (8000000UL/(alarmarr[0]*2));
			}
		}
		if (t == period[0]){
			if (alarmarr[1] == 0){
				OCR1A = 0;
			} else {
				OCR1A = (8000000UL/(alarmarr[1]*2));
			}
		}
		if (t == period[1]){
			if (alarmarr[2] == 0){
				OCR1A = 0;
			} else {
				OCR1A = (8000000UL/(alarmarr[2]*2));
			}
		}
		if (t == period[2]){
			if (alarmarr[3] == 0){
				OCR1A = 0;
			} else {
				OCR1A = (8000000UL/(alarmarr[3]*2));
			}
		}
		if (t == period[3]){
			OCR1A = 0;
			t = 0;
		}
	

		if (t >= 0){
			t+= 2;
		}
		
	}
}

/*
 * Interrupt Routine.
 */
ISR(USART_RX_vect){
	int c;
	c = USART_Receive();

	if (c == 61) {
		testfunc();
		timerFlag = 1;
	}
	if (c == 62) {
		getAlarmFreq();
	}
	if (c == 65) {
		setAlarmTime(1);
	}
	if (c == 66) {
		setAlarmTime(2);
	}
	if (c == 67) {
		setAlarmTime(3);
	}
	if (c == 68) {
		setAlarmTime(4);
	}
	
}

/*
The ADXL343 will be set up and and send data to detect which mode the while loop will be in. 
Mode implements Functionality, switching between four possible modes.
*/	
int main(void)
{		
	init_chip();
	ADXL343_setup();
	ADXL343_interrupt_setup();
	u8g2_Setup_st7920_s_128x64_f( &u8g2, U8G2_R0, u8x8_byte_avr_hw_spi, u8x8_avr_gpio_and_delay );
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2, u8g2_font_fub20_tf );
	u8g2_DrawStr(&u8g2, 1, 18, "GROUP07");
	u8g2_SendBuffer(&u8g2);
	u8g2_ClearBuffer(&u8g2);                       
	timer2_setup();
	
	int i = 0;                   // Dummy value counter for humidity and temp sensor
	while(1){
		if(AlarmTime[0] == timearr[0] &&
		AlarmTime[1] == timearr[1] &&
		AlarmTime[2] == timearr[2]){
			alarmFlag = 1;
		}
		if(AlarmTime[3] == timearr[0] &&
		AlarmTime[4] == timearr[1] &&
		AlarmTime[5] == timearr[2]){
			alarmFlag = 1;
		}
		if(AlarmTime[6] == timearr[0] &&
		AlarmTime[7] == timearr[1] &&
		AlarmTime[8] == timearr[2]){
			alarmFlag = 1;
		}
		if(AlarmTime[9] == timearr[0] &&
		AlarmTime[10] == timearr[1] &&
		AlarmTime[11] == timearr[2]){
			alarmFlag = 1;
		}
		
		/*
		Setup here, will find the mode using ADXL343 calculations to find if it's being rotated
		then print the appropriate thing and values stored. 
		*/
		mode = ADXL343_calc_mode(ADXL343_read_x(), ADXL343_read_y());
		tapStatus = ADXL343_check_tap();
		
		if (past_mode != mode){
			if (mode == 1){
				stopFlag = 1;
				u8g2_Setup_st7920_s_128x64_f( &u8g2, U8G2_R2, u8x8_byte_avr_hw_spi, u8x8_avr_gpio_and_delay );
				u8g2_InitDisplay(&u8g2);
				u8g2_SetPowerSave(&u8g2, 0);
				u8g2_SetFont(&u8g2, u8g2_font_fub11_tf );
				
			}
			if (mode == 2){
				u8g2_Setup_st7920_s_128x64_f( &u8g2, U8G2_R1, u8x8_byte_avr_hw_spi, u8x8_avr_gpio_and_delay );
				u8g2_InitDisplay(&u8g2);
				u8g2_SetPowerSave(&u8g2, 0);
				u8g2_SetFont(&u8g2, u8g2_font_fub11_tf);
				
			}
			if (mode == 3){
				u8g2_Setup_st7920_s_128x64_f( &u8g2, U8G2_R0, u8x8_byte_avr_hw_spi, u8x8_avr_gpio_and_delay );
				u8g2_InitDisplay(&u8g2);
				u8g2_SetPowerSave(&u8g2, 0);
				u8g2_SetFont(&u8g2, u8g2_font_fub11_tf );		
				} 
				
			if (mode == 4) {
				u8g2_Setup_st7920_s_128x64_f( &u8g2, U8G2_R3, u8x8_byte_avr_hw_spi, u8x8_avr_gpio_and_delay );
				u8g2_InitDisplay(&u8g2);
				u8g2_SetPowerSave(&u8g2, 0);
				u8g2_SetFont(&u8g2, u8g2_font_6x13_tr  );
			}
			if (mode == 0) {
				mode = past_mode;
			}

		}		
		
		//Clock Mode
		if (mode == 4) {
			sprintf(str, "%02d:%02d:%02d", timearr[0], timearr[1], timearr[2]);
			u8g2_ClearBuffer(&u8g2);
			u8g2_DrawStr(&u8g2, 8, 32, str);
			
			if (timearr[0] >= 12) {
				u8g2_DrawStr(&u8g2, 26, 55, night);
			}
			else {
				u8g2_DrawStr(&u8g2, 26, 55, day);
			}
			
			if (timerSet == 0) {
				u8g2_DrawStr(&u8g2, 1, 127, "No alarm");
			}
			if (timerSet == 1){
				u8g2_DrawStr(&u8g2, 1, 127, "Alarm here.");
			}
			if (timerFlag == 0){
				u8g2_DrawStr(&u8g2, 1, 10, "No Time");
			}
			if (tapStatus == 1){
				alarmFlag = 0;
			}
			u8g2_SendBuffer(&u8g2);
		}
		
		//Stopwatch Mode
		if (mode == 1) {
			u8g2_ClearBuffer(&u8g2);
			
			if (tapStatus == 2 && past_mode == mode) {
				stoparr[0] = 0;
				stoparr[1] = 0;
				stoparr[2] = 0;
				stoparr[3] = 0;
			}
			if (tapStatus == 1 && past_mode == mode) {
				if (stopFlag == 0){
					stopFlag = 1;
				} else {
					stopFlag = 0;
				}
			}			
			u8g2_DrawStr(&u8g2, 1, 10, "Stopwatch");
			sprintf(str, "%02d:%02d:%02d:%01d", stoparr[0], stoparr[1], stoparr[2], stoparr[3]);
			u8g2_DrawStr(&u8g2, 1, 40, str);
			u8g2_SendBuffer(&u8g2);
				
		}
		
		//Battery Voltage Mode
		if (mode == 3) {
			init_ADC1();
			voltageValue = update_battery_voltage(); //read ADC value in
			floatVolts = (float)voltageValue/(205.2) - 0.1;
			sprintf(str, "%d.%02d V",(int)floatVolts, (int)(floatVolts*100)%100);
			if (millis % 300 == 0) {
				u8g2_ClearBuffer(&u8g2);
				u8g2_DrawStr(&u8g2, 1, 18, "Battery");
				u8g2_DrawStr(&u8g2, 1, 40, str);
				u8g2_SendBuffer(&u8g2);
			}
		} 
		
		//Humidity/Temp Mode
		if (mode == 2) {
			if (millis % 500 == 0) {
				u8g2_ClearBuffer(&u8g2);
				sprintf(str, "%02d.%01d", humidityDummies[i], humidityDecs[i]);
				u8g2_DrawStr(&u8g2, 1, 20, str);
				u8g2_DrawStr(&u8g2, 1, 35, "%RH");
			
			
				sprintf(str, "%d.%01d %cC", tempDummies[i], tempDecs[i], 176);

				u8g2_DrawStr(&u8g2, 1, 64, str);
				u8g2_SendBuffer(&u8g2);
			
				i++;
				if (i == 4){
					i = 0;
				}
			}		
		}
		past_mode = mode;		
	}
}

