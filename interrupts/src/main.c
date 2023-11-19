#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

// Register logging module
LOG_MODULE_REGISTER(interrupt_test);

// Threads
#define BUTTON_STACK_SIZE 500
#define BUTTON_THREAD_PRIORITY -5 // FIFO to ensure complete execution

K_THREAD_STACK_DEFINE(thread_a_stack_area, BUTTON_STACK_SIZE);
struct k_thread button_thread_data;

void button_thread_function(void *p1, void *p2, void *p3);

// GPIO defines
#define SW0_NODE	DT_ALIAS(sw0)
static struct gpio_dt_spec userbutton = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});

static struct gpio_callback button_cb_data;

void button_callback(const struct device *dev, 
                        struct gpio_callback *cb,
		                uint32_t pins);

int main(void)
{
    // setup logging
    LOG_MODULE_DECLARE(interrupt_test);

    /* configure GPIO */
    // User button
	if (!device_is_ready(userbutton.port)) {
		LOG_ERR("Error: button device %s is not ready\n", userbutton.port->name);
		return -1;
	} else {
        // configure pin
        int ret = gpio_pin_configure_dt(&userbutton, GPIO_INPUT);
        if (ret != 0) {
            LOG_ERR("Error %d: failed to configure %s pin %d\n", ret, userbutton.port->name, userbutton.pin);
            return -1;
        } else {
            LOG_INF("Configured %s pin %d\n",userbutton.port->name, userbutton.pin);
        }
        
        // configure interrupt
        ret = gpio_pin_interrupt_configure_dt(&userbutton, GPIO_INT_EDGE_TO_ACTIVE);
        if(ret != 0){
            LOG_ERR("Error %d: failed to configure interrupt on %s pin %d\n", ret, userbutton.port->name, userbutton.pin);    
        }

        // initialize and add callback
        gpio_init_callback(&button_cb_data, button_callback, BIT(userbutton.pin));
        gpio_add_callback(userbutton.port, &button_cb_data);
	    LOG_INF("Set up button as interrupt at %s pin %d\n", userbutton.port->name, userbutton.pin);
    }

    /* create threads*/
    k_tid_t thread_a_tid = k_thread_create(&button_thread_data, thread_a_stack_area,
                                 K_THREAD_STACK_SIZEOF(thread_a_stack_area),
                                 button_thread_function,
                                 NULL, NULL, NULL,
                                 BUTTON_THREAD_PRIORITY, 0, K_FOREVER);
    k_thread_name_set(&button_thread_data, "thread_a");        
    k_thread_start(&button_thread_data);
    /* Main loop*/
    
    while(true){
        k_msleep(500);
    }
	return 0;
}

// FIFO thread
void button_thread_function(void *p1, void *p2, void *p3){
    printk("Thread starting\n");  
    while(1){
        // wait until interrupt removes suspension
        k_thread_suspend(k_current_get());

        // unneccessarily fancy printing
        printk("Button pressed\n");
        printk("Doing stuff.");
        for(int i = 0; i<3; i++){
            k_msleep(500);
            printk("...");
        }
        printk("\n");
    } 
}

void button_callback(const struct device *dev, 
                        struct gpio_callback *cb,
		                uint32_t pins){

    k_thread_resume(&button_thread_data);
}