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

void gpio_configure(const struct gpio_dt_spec gpio_spec, gpio_flags_t gpio_flag);

/* PWM */
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

#define MIN_PERIOD PWM_SEC(1U) / 128U
#define MAX_PERIOD 31250000 //PWM_SEC(1U)

int main(void)
{
	uint32_t pulse_width;
	int ret;

	printk("\nPWM-based stepper driver\n");
	printk("Starting GPIO/PWM setup\n");

	/* GPIO configuration */
	gpio_configure(enable, GPIO_OUTPUT);
	gpio_configure(direction, GPIO_OUTPUT);

	/* PWM configuration */
	if (!pwm_is_ready_dt(&pwm_led0)) {
		printk("Error: PWM device %s is not ready\n",
		       pwm_led0.dev->name);
		return 0;
	}

	printk("Done calibrating; maximum/minimum periods %u/%lu nsec\n",
	       MAX_PERIOD, MIN_PERIOD);

	pulse_width = MIN_PERIOD;
	while (1) {
        
		// set pulse width
		ret = pwm_set_dt(&pwm_led0, MIN_PERIOD, pulse_width);
		if (ret) {
			printk("Error %d: failed to set pulse width\n", ret);
			return 0;
		}

		// fade LED
		// pulse_width -= period/100;
		// printk("pulse width: %d\n", pulse_width);

		// if(pulse_width <= period/100){
		// 	pulse_width = period;
		// }

		// k_sleep(K_MSEC(30U));

		k_sleep(K_MSEC(500));
	}
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

