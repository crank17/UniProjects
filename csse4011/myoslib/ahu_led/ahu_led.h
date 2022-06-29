/**
 ************************************************************************
 * @file ahu_led.h
 * @author Ryan Ward
 * @date 31.03.2022
 * @brief Contains Macros and Definitions for handling LEDs on NRF Dongle
 **********************************************************************
 * */
#ifndef AHU_LED
#define AHU_LED
/*
 * Extract devicetree configuration.
 */
#include <zephyr.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/pwm.h>
#include "ahu_shell.h"
/* ------------- GET LED CHANNELS ------------- */
#define RED_NODE DT_ALIAS(red_pwm_led)
#define GREEN_NODE DT_ALIAS(green_pwm_led)
#define BLUE_NODE DT_ALIAS(blue_pwm_led)

#if DT_NODE_HAS_STATUS(RED_NODE, okay)
#define RED_CTLR_NODE	DT_PWMS_CTLR(RED_NODE)
#define RED_CHANNEL	DT_PWMS_CHANNEL(RED_NODE)
#define RED_FLAGS	DT_PWMS_FLAGS(RED_NODE)
#else
#error "Unsupported board: red-pwm-led devicetree alias is not defined"
#define RED_CTLR_NODE	DT_INVALID_NODE
#define RED_CHANNEL	0
#define RED_FLAGS	0
#endif

#if DT_NODE_HAS_STATUS(GREEN_NODE, okay)
#define GREEN_CTLR_NODE	DT_PWMS_CTLR(GREEN_NODE)
#define GREEN_CHANNEL	DT_PWMS_CHANNEL(GREEN_NODE)
#define GREEN_FLAGS	DT_PWMS_FLAGS(GREEN_NODE)
#else
#error "Unsupported board: green-pwm-led devicetree alias is not defined"
#define GREEN_CTLR_NODE	DT_INVALID_NODE
#define GREEN_CHANNEL	0
#define GREEN_FLAGS	0
#endif

#if DT_NODE_HAS_STATUS(BLUE_NODE, okay)
#define BLUE_CTLR_NODE	DT_PWMS_CTLR(BLUE_NODE)
#define BLUE_CHANNEL	DT_PWMS_CHANNEL(BLUE_NODE)
#define BLUE_FLAGS	DT_PWMS_FLAGS(BLUE_NODE)
#else
#error "Unsupported board: blue-pwm-led devicetree alias is not defined"
#define BLUE_CTLR_NODE	DT_INVALID_NODE
#define BLUE_CHANNEL	0
#define BLUE_FLAGS	0
#endif
/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS	DT_GPIO_FLAGS(LED0_NODE, gpios)
#else
/* A build error here means your board isn't set up to blink an LED. */
#error "Unsupported board: led0 devicetree alias is not defined"
#define LED0	""
#define PIN	0
#define FLAGS	0
#endif
/*-------------------------------------------------------------- */
/* LED  */
#define PERIOD_USEC	(USEC_PER_SEC / 50U)
#define STEPSIZE_USEC	2000

/* LED Stack size and priority */
#define LED_STACK_SIZE 512
#define LED_PRIORITY 3

/* Blink thread stack size and priority */
#define BLINKY_STACK_SIZE 512
#define BLINKY_PRIORITY 5

/* fifo struct */
extern struct k_fifo my_led_fifo;

/* Function signatures */
void led_thread_entry(void);
void blinky_thread(void);

#endif

