#include <stdio.h>
#include <stdbool.h>    // booleans
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>


// GPIO defines
#define SW0_NODE	DT_ALIAS(sw0)
#define LED_NODE	DT_ALIAS(led0)
//#define LED_NODE	DT_NODELABEL(led0)
static struct gpio_dt_spec userbutton = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_dt_spec userled = GPIO_DT_SPEC_GET_OR(LED_NODE, gpios, {0});

int main(void)
{
    /* configure GPIO */ 

    // User button
	if (!device_is_ready(userbutton.port)) {
		printk("Error: button device %s is not ready\n", userbutton.port->name);
		return -1;
	} else {
        int ret = gpio_pin_configure_dt(&userbutton, GPIO_INPUT);
        if (ret != 0) {
            printk("Error %d: failed to configure %s pin %d\n", ret, userbutton.port->name, userbutton.pin);
            return -1;
        } else {
            printk("Configured %s pin %d\n",userbutton.port->name, userbutton.pin);
        }
    }

    // User led
    //printk("LED status: %s\n",DT_PROP(LED_NODE, status));
    if(!userled.port){
        printk("Error: led device %s is not ready\n", userled.port->name);
		return -1;
    } else {
        int ret_led = gpio_pin_configure_dt(&userled, GPIO_OUTPUT);
        if (ret_led != 0) {
            printk("Error %d: failed to configure %s pin %d\n", ret_led, userled.port->name, userled.pin);
            return -1;
        } else {
            printk("Configured %s pin %d\n",userled.port->name, userled.pin);
        }
    }
    
    //gpio_pin_interrupt_configure_dt(&userbutton, GPIO_INT_EDGE_RISING);

	printf("GPIO configuration successful\n");



    /* Main loop*/
    int i = 0.0;
    
    while(true){
        int userbutton_state = gpio_pin_get_dt(&userbutton);
        gpio_pin_set_dt(&userled, userbutton_state);
        printk("User button reads %x \n", userbutton_state);
        i++;
        k_msleep(500);
    }
	return 0;
}
