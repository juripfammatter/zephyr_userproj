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
    /*!
     * @brief Constructor
     * @param step_divider (possible values [2, 4, 8, 16])
     */
    explicit Stepper(double step_divider);

    /*!
     * @brief Deconstructor
     * @param none
     */
    virtual ~Stepper(void);

    /*!
     * @brief Enable stepper
     * @param none
     */
    void enable_stepper(void);

    /*!
     * @brief Disable stepper
     * @param none
     */
    void disable_stepper(void);

    /*!
     * @brief Set velocity
     * @param vel in steps/s
     */
    void set_velocity(double vel);

    /*!
     * @brief  measure postion of stepper
     * @param  none
     * @return position in steps
     */
    double get_position(void);

private:
    /* Variables */
    double phi_steps; // angle in steps

    double step_size; // °/step
    double step_divider;

    double current_speed; // steps/s
    double max_speed;     // steps/s

    bool stepper_enabled;

    /* Threading */
    struct k_thread stepper_thread;

    /*!
     * @brief Stepper thread entry (C/C++ trampoline)
     * @param stepper object
     * @param
     * @param
     */
    static void stepper_thread_entry(void *, void *, void *);

    /*!
     * @brief Thread that creates steps based on current_speed
     * @param none
     */
    void stepper_thread_function(void);

    /*!
     * @brief GPIO configuration
     * @param gpio_spec: gpio_dt_spec struct
     * @param gpio_flag: Flags (GPIO_OUTPUT | GPIO_INPUT)
     */
    void gpio_configure(const struct gpio_dt_spec gpio_spec, gpio_flags_t gpio_flag);
};