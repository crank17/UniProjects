/**
 ************************************************************************
 * @file ahu_led.c
 * @author Ryan Ward
 * @date 31.03.2022
 * @brief Contains Source code for handling LEDs on NRF Dongle
 **********************************************************************
 * */


#include "./ahu_led.h"
 /* Register LOG Module */
LOG_MODULE_REGISTER(led_module, LOG_LEVEL_DBG);


/* Funciton to set PWM for RGB LEDs */
static int pwm_set(const struct device* pwm_dev, uint32_t pwm_pin,
	uint32_t pulse_width, pwm_flags_t flags)
{
	return pwm_pin_set_usec(pwm_dev, pwm_pin, PERIOD_USEC,
		pulse_width, flags);
}

/* Toggle mode */
static uint64_t check_mode(uint8_t colour1, uint8_t colour2, uint8_t mode, uint64_t currentMode) {
	if (colour1 == colour2 && mode == ON) {
		return 10000;
	}
	else if (colour1 == colour2 && mode == OFF) {
		return 0;
	}
	return currentMode;

}

/* Thread to handle display of RGB LED */
void led_thread_entry(void) {
	const struct device* pwm_dev[3];
	struct data_item_t* rx_data;
	int ret;
	uint8_t colour, mode;
	printk("PWM-based RGB LED control\n");

	pwm_dev[RED] = DEVICE_DT_GET(RED_CTLR_NODE);
	pwm_dev[GREEN] = DEVICE_DT_GET(GREEN_CTLR_NODE);
	pwm_dev[BLUE] = DEVICE_DT_GET(BLUE_CTLR_NODE);

	if (!device_is_ready(pwm_dev[RED])) {
		printk("Error: red PWM device %s is not ready\n", pwm_dev[RED]->name);
		return;
	}

	if (!device_is_ready(pwm_dev[GREEN])) {
		printk("Error: green PWM device %s is not ready\n", pwm_dev[GREEN]->name);
		return;
	}

	if (!device_is_ready(pwm_dev[BLUE])) {
		printk("Error: blue PWM device %s is not ready\n", pwm_dev[BLUE]->name);
		return;
	}
	uint64_t redmode = 0;
	uint64_t bluemode = 0;
	uint64_t greenmode = 0;
	while (1) {
		rx_data = k_fifo_get(&my_led_fifo, K_FOREVER);
		colour = rx_data->colour;
		mode = rx_data->mode;
		redmode = check_mode(colour, RED, mode, redmode);
		greenmode = check_mode(colour, GREEN, mode, greenmode);
		bluemode = check_mode(colour, BLUE, mode, bluemode);
		if (mode == TOGGLE) {
			switch (colour) {
			case RED:
				redmode = (redmode == 10000) ? 0 : 10000;
				break;
			case BLUE:
				bluemode = (bluemode == 10000) ? 0 : 10000;
				break;
			case GREEN:
				greenmode = (greenmode == 10000) ? 0 : 10000;
				break;
			}
		}
		ret = pwm_set(pwm_dev[RED], RED_CHANNEL, redmode, RED_FLAGS);
		ret = pwm_set(pwm_dev[BLUE], BLUE_CHANNEL, bluemode, BLUE_FLAGS);
		ret = pwm_set(pwm_dev[GREEN], GREEN_CHANNEL, greenmode, GREEN_FLAGS);
		k_sleep(K_SECONDS(1));
	}

}

/* Keep alive thread */
void blinky_thread(void) {
	static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
	int ret;
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

	if (ret < 0) {
		return;
	}

	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return;
		}

		k_sleep(K_SECONDS(1));
	}
}
