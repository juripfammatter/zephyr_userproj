/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>

/* GPIOs */
#define EN_DIR_NODE 	DT_ALIAS(en_dir)
// #define DIR_NODE	DT_ALIAS(direction)

static struct gpio_dt_spec enable = GPIO_DT_SPEC_GET_BY_IDX(EN_DIR_NODE, gpios, 0);
static struct gpio_dt_spec direction = GPIO_DT_SPEC_GET_BY_IDX(EN_DIR_NODE, gpios, 1);
static struct gpio_dt_spec step = GPIO_DT_SPEC_GET_BY_IDX(EN_DIR_NODE, gpios, 2);

void gpio_configure(const struct gpio_dt_spec gpio_spec, gpio_flags_t gpio_flag);

/* PWM */
// static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

// #define MIN_PERIOD PWM_SEC(1U) / 50U // 50Hz
// #define MAX_PERIOD 31250000 //PWM_SEC(1U)

int main(void)
{
	uint32_t pulse_width;
	int ret;

	// printk("\nPWM-based stepper driver\n");
	printk("Starting GPIO setup\n");

	/* GPIO configuration */
	gpio_configure(enable, GPIO_OUTPUT);
	gpio_configure(direction, GPIO_OUTPUT);
	gpio_configure(step, GPIO_OUTPUT);



	// /* PWM configuration */
	// if (!pwm_is_ready_dt(&pwm_led0)) {
	// 	printk("Error: PWM device %s is not ready\n",
	// 	       pwm_led0.dev->name);
	// 	return 0;
	// }

	// printk("Done calibrating; maximum/minimum periods %u/%lu nsec\n",
	//        MAX_PERIOD, MIN_PERIOD);

	// set initial conditions
	gpio_pin_set_dt(&enable, 0);
	gpio_pin_set_dt(&direction, 0);
	gpio_pin_set_dt(&step, 0);

	// ret = pwm_set_dt(&pwm_led0, MIN_PERIOD, 0);
	// if (ret) {
	// 	printk("Error %d: failed to set pulse width\n", ret);
	// 	return 0;
	// }

	k_sleep(K_MSEC(500));

	// pulse_width = MIN_PERIOD/2;
	// pulse_width = 0;
        
	gpio_pin_set_dt(&enable, 1);
	gpio_pin_set_dt(&direction, 1);

	k_sleep(K_MSEC(500));

	// rotate
	int i=0;
	for(i=0; i<(200*4); i++){
		gpio_pin_set_dt(&step, 1);
		k_sleep(K_USEC(250/4));
		gpio_pin_set_dt(&step, 0);
		k_sleep(K_USEC(250/4));
	}
	// set pulse width
	// ret = pwm_set_dt(&pwm_led0, MIN_PERIOD, pulse_width);
	// if (ret) {
	// 	printk("Error %d: failed to set pulse width\n", ret);
	// 	return 0;
	// }

	// k_sleep(K_MSEC(5000));
	// set pulse width
	// ret = pwm_set_dt(&pwm_led0, MIN_PERIOD, 0);
	// if (ret) {
	// 	printk("Error %d: failed to set pulse width\n", ret);
	// 	return 0;
	// }

	k_sleep(K_MSEC(500));

	gpio_pin_set_dt(&enable, 0);
	gpio_pin_set_dt(&direction, 0);
	
	return 0;
}




void gpio_configure(const struct gpio_dt_spec gpio_spec, gpio_flags_t gpio_flag){
	if (!device_is_ready(gpio_spec.port)) {
		printk("Error: button device %s is not ready\n", gpio_spec.port->name);
		// return -1;
	} else {
        int ret = gpio_pin_configure_dt(&gpio_spec, gpio_flag);
        if (ret != 0) {
            printk("Error %d: failed to configure %s pin %d to %d\n", ret, gpio_spec.port->name, gpio_spec.pin, gpio_spec.dt_flags);
            // return -1;
        } else {
            printk("Configured %s pin %d to %d\n",gpio_spec.port->name, gpio_spec.pin, gpio_spec.dt_flags);
        }
    }
}

