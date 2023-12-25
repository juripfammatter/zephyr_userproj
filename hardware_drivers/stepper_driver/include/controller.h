#pragma once
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <queue>
#include "stepper.h"
#include "math.h"

#define BUFFER_LENGTH 300
#define CONTROL_LOOP_STACK_SIZE 512
#define CONTROL_LOOP_PRIORITY -5 // FIFO thread

class Controller
{

public:
    /*!
     * @brief Constructor
     * @param kp
     * @param sample_time
     */
    explicit Controller(double kp, double sample_time);

    /*!
     * @brief Deconstructor
     * @param none
     */
    virtual ~Controller(void);

    /*!
     * @brief print latest buffer // don't dump while controller is running
     * @param
     */
    void dump_buffer(void);

    /*!
     * @brief set value for velocity control
     * @param vel
     */
    void set_velocity(double vel);

    /*!
     * @brief set value for position control
     * @param ref
     */
    void set_reference(double ref);

private:
    double reference;
    double error;
    double kp;
    double sample_time; // usec

    /* measurement buffer */
    std::queue<double> buffer;

    /* stepper object */
    Stepper stepper;

    /* Timer */
    // struct k_timer control_loop_timer;

    /* Threading */
    struct k_thread control_loop_thread;

    /*!
     * @brief control loop trampoline
     * @param instance pointer
     * @param none
     * @param none
     */
    static void control_loop_thread_entry(void *, void *, void *);

    /*!
     * @brief control loop (running in separate thread)
     * @param none
     */
    void control_loop_thread_function(void);
};