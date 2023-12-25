#pragma once
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <queue>
#include "stepper.h"

#define BUFFER_LENGTH 20
#define CONTROL_LOOP_STACK_SIZE 512
#define CONTROL_LOOP_PRIORITY -5 //FIFO thread

class Controller
{

public:
    explicit Controller(double kp, double sample_time);

    virtual ~Controller(void);

    void dump_buffer(void);

    void set_velocity(double vel);

private:
    double reference;
    double kp;
    double sample_time; //usec

    /* measurement buffer */
    std::queue<double> buffer;

    /* stepper object */
    Stepper stepper;

    /* Timer */
    // struct k_timer control_loop_timer;

    /* Threading */
    struct k_thread control_loop_thread;

    static void control_loop_thread_entry(void *, void *, void *);

    void control_loop_thread_function(void);
};