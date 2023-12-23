#include "stepper.h"

/* Threading */
K_THREAD_STACK_DEFINE(stepper_thread_stack_area, STEPPER_STACK_SIZE);

/* GPIOs */
static struct gpio_dt_spec enable = GPIO_DT_SPEC_GET_BY_IDX(EN_DIR_NODE, gpios, 0);
static struct gpio_dt_spec direction = GPIO_DT_SPEC_GET_BY_IDX(EN_DIR_NODE, gpios, 1);
static struct gpio_dt_spec step = GPIO_DT_SPEC_GET_BY_IDX(EN_DIR_NODE, gpios, 2);

/* Logging*/
LOG_MODULE_REGISTER(logger);

// public:
Stepper::Stepper(double step_divider) : phi_steps(0),
                                        step_size(360.0 / (200.0 * step_divider)),
                                        step_divider(step_divider),
                                        current_speed(0),
                                        stepper_enabled(false)

{
    // GPIO configuration
    Stepper::gpio_configure(enable, GPIO_OUTPUT);
    Stepper::gpio_configure(direction, GPIO_OUTPUT);
    Stepper::gpio_configure(step, GPIO_OUTPUT);

    // set initial conditions
    gpio_pin_set_dt(&enable, 0);
    gpio_pin_set_dt(&direction, 0);
    gpio_pin_set_dt(&step, 0);

    // create and start thread
    k_tid_t stepper_thread_tid = k_thread_create(&stepper_thread, stepper_thread_stack_area,
                                                 K_THREAD_STACK_SIZEOF(stepper_thread_stack_area),
                                                 Stepper::stepper_thread_entry,
                                                 this, NULL, NULL,
                                                 STEPPER_THREAD_PRIORITY, 0, K_MSEC(10));

    LOG_MODULE_DECLARE(logger);
    LOG_INF("Initialized stepper with step size %.3f° and step divider %0.1f\n", step_size, step_divider);
    LOG_INF("Stepper thread running\n");
}

Stepper::~Stepper(void)
{
    Stepper::disable_stepper();
}

void Stepper::enable_stepper(void)
{
    gpio_pin_set_dt(&enable, 1);
    stepper_enabled = true;
}

void Stepper::disable_stepper(void)
{
    gpio_pin_set_dt(&enable, 0);
    stepper_enabled = false;
}

void Stepper::set_velocity(double vel)
{
    current_speed = vel * step_divider;

    LOG_MODULE_DECLARE(logger);
    LOG_INF("Setting speed to %.3f (divider = %.1f)\n", current_speed, step_divider);
}

double Stepper::get_position(void)
{
    return phi_steps / step_divider;
}

// private:
void Stepper::stepper_thread_entry(void *instance, void *, void *)
{
    auto stepper_thead_instance = reinterpret_cast<Stepper *>(instance);
    stepper_thead_instance->stepper_thread_function();
}

void Stepper::stepper_thread_function(void)
{
    while (1)
    {
        double desired_speed = current_speed;

        if (desired_speed != 0 && stepper_enabled)
        {
            if (desired_speed < 0)
            {
                gpio_pin_set_dt(&direction, 1);
            }
            else
            {
                gpio_pin_set_dt(&direction, 0);
            }

            // calculate period in usec
            int period = 1000000.0 / fabs(desired_speed);
            period -= (int)(1277 / step_divider); // correct for time of gpio setting

            // set speed
            gpio_pin_set_dt(&step, 1);
            k_sleep(K_USEC(period / 2));

            gpio_pin_set_dt(&step, 0);
            k_sleep(K_USEC(period / 2));

            // increment/ decrement position
            (desired_speed > 0) ? phi_steps++ : phi_steps--;
        }
        else
        {
            gpio_pin_set_dt(&step, 0);
            k_sleep(K_USEC(10));
        }
    }
}

void Stepper::gpio_configure(const struct gpio_dt_spec gpio_spec, gpio_flags_t gpio_flag)
{
    LOG_MODULE_DECLARE(test);
    if (!device_is_ready(gpio_spec.port))
    {
        LOG_ERR("Error: button device %s is not ready\n", gpio_spec.port->name);
    }
    else
    {
        int ret = gpio_pin_configure_dt(&gpio_spec, gpio_flag);
        if (ret != 0)
        {
            LOG_ERR("Error %d: failed to configure %s pin %d to %d\n", ret, gpio_spec.port->name, gpio_spec.pin, gpio_spec.dt_flags);
        }
        else
        {
            LOG_INF("Configured %s pin %d to %d\n", gpio_spec.port->name, gpio_spec.pin, gpio_spec.dt_flags);
        }
    }
}