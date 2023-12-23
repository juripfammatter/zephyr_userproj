#pragma once
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <math.h>

#define STEPPER_STACK_SIZE 512
#define STEPPER_THREAD_PRIORITY -5 // FIFO thread

#define EN_DIR_NODE DT_ALIAS(en_dir)

/* Stepper class */
class Stepper
{

public:
    /* Constructor */
    explicit Stepper(double step_divider);

    /* Deconstructor */
    virtual ~Stepper(void);

    /* Enable stepper */
    void enable_stepper(void);

    /* Disable stepper*/
    void disable_stepper(void);

    /* set velocity in steps/s */
    void set_velocity(double vel);
    
    /* get angle in steps */
    double get_position(void);

private:
    /* Variables */
    double phi_steps; // angle in steps

    double step_size; // °/step
    double step_divider;

    double current_speed; // steps/s
    double max_speed;     // steps/s

    /* Threading */
    struct k_thread stepper_thread;

    /* Stepper thread entry */
    static void stepper_thread_entry(void *, void *, void *);

    /* Stepper thread function */
    void stepper_thread_function(void);

    /* GPIO configuration */
    void gpio_configure(const struct gpio_dt_spec gpio_spec, gpio_flags_t gpio_flag);
};