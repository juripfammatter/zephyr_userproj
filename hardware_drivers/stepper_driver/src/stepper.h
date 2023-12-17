#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <cstdlib>

#define STEPPER_STACK_SIZE 512
#define STEPPER_THREAD_PRIORITY -5 // FIFO thread

/* Threading */
K_THREAD_STACK_DEFINE(stepper_thread_stack_area, STEPPER_STACK_SIZE);

/* GPIOs */
#define EN_DIR_NODE DT_ALIAS(en_dir)

static struct gpio_dt_spec enable = GPIO_DT_SPEC_GET_BY_IDX(EN_DIR_NODE, gpios, 0);
static struct gpio_dt_spec direction = GPIO_DT_SPEC_GET_BY_IDX(EN_DIR_NODE, gpios, 1);
static struct gpio_dt_spec step = GPIO_DT_SPEC_GET_BY_IDX(EN_DIR_NODE, gpios, 2);

/* Class */

class Stepper
{
    double phi_steps; // angle °

    double step_size; // °/step
    double step_divider;

    double current_speed; // °/s
    double max_speed;     // °/s

public:
    Stepper(double step_divider) : step_size(360.0 / (200.0 * step_divider)),
                                   step_divider(step_divider),
                                   current_speed(0)
    {
        /* GPIO configuration */
        gpio_configure(enable, GPIO_OUTPUT);
        gpio_configure(direction, GPIO_OUTPUT);
        gpio_configure(step, GPIO_OUTPUT);

        // set initial conditions
        gpio_pin_set_dt(&enable, 0);
        gpio_pin_set_dt(&direction, 0);
        gpio_pin_set_dt(&step, 0);

        // create and start thread
        k_tid_t stepper_thread_tid = k_thread_create(&stepper_thread, stepper_thread_stack_area,
                                                     K_THREAD_STACK_SIZEOF(stepper_thread_stack_area),
                                                     stepper_thread_entry,
                                                     this, NULL, NULL,
                                                     STEPPER_THREAD_PRIORITY, 0, K_SECONDS(1));

        printk("Initialized stepper with step size %.3f°\n", step_size);
        printk("Stepper thread running\n");
    };

    void enable_stepper(void)
    {
        gpio_pin_set_dt(&enable, 1);
    }

    void disable_stepper(void)
    {
        gpio_pin_set_dt(&enable, 0);
    }

    // set velocity in °/s
    void set_velocity(double vel)
    {
        this->current_speed = vel;
    }

    double get_position(void)
    {
        return phi_steps * step_size;
    }

private:
    /* Threading */
    struct k_thread stepper_thread;

    /* Stepper thread entry */
    static void stepper_thread_entry(void *instance, void *, void *)
    {
        auto stepper_thead_instance = static_cast<Stepper *>(instance);
        stepper_thead_instance->stepper_thread_function();
    }

    /* Stepper thread function */
    void stepper_thread_function(void)
    {
        while (1)
        {
            if (current_speed != 0)
            {
                if (current_speed < 0)
                {
                    gpio_pin_set_dt(&direction, 1);
                }
                else
                {
                    gpio_pin_set_dt(&direction, 0);
                }
                // calculate period in usec
                int period = step_size * 1000000 / abs(current_speed);

                // set speed
                gpio_pin_set_dt(&step, 1);
                k_sleep(K_USEC(period / 2));
                gpio_pin_set_dt(&step, 0);
                k_sleep(K_USEC(period / 2));
            }
            else
            {
                gpio_pin_set_dt(&step, 0);
                k_sleep(K_MSEC(10));
            }
        }
    }

    /* GPIO configuration */
    void gpio_configure(const struct gpio_dt_spec gpio_spec, gpio_flags_t gpio_flag)
    {
        if (!device_is_ready(gpio_spec.port))
        {
            printk("Error: button device %s is not ready\n", gpio_spec.port->name);
            // return -1;
        }
        else
        {
            int ret = gpio_pin_configure_dt(&gpio_spec, gpio_flag);
            if (ret != 0)
            {
                printk("Error %d: failed to configure %s pin %d to %d\n", ret, gpio_spec.port->name, gpio_spec.pin, gpio_spec.dt_flags);
                // return -1;
            }
            else
            {
                printk("Configured %s pin %d to %d\n", gpio_spec.port->name, gpio_spec.pin, gpio_spec.dt_flags);
            }
        }
    }
};
