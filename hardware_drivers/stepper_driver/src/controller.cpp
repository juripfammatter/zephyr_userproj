#include "controller.h"

/* Timer */
K_TIMER_DEFINE(control_loop_timer, NULL, NULL);

/* Threading */
K_THREAD_STACK_DEFINE(control_loop_thread_area, CONTROL_LOOP_STACK_SIZE);

// public
Controller::Controller(double kp, double sample_time) : reference(0),
                                                        error(0),
                                                        kp(kp),
                                                        sample_time(sample_time),
                                                        stepper(4)

{
    // fill buffer with zeros
    for (unsigned i = 0; i < BUFFER_LENGTH; i++)
    {
        buffer.emplace(0);
    }

    // enable stepper
    stepper.enable_stepper();

    // start timer based on sample time
    // k_timer_start(&control_loop_timer, K_NO_WAIT, K_USEC(sample_time));

    // create thread
    k_tid_t control_loop_thread_tid = k_thread_create(&control_loop_thread, control_loop_thread_area,
                                                      K_THREAD_STACK_SIZEOF(control_loop_thread_area),
                                                      Controller::control_loop_thread_entry,
                                                      this, NULL, NULL,
                                                      CONTROL_LOOP_PRIORITY, 0, K_NO_WAIT);
}

Controller::~Controller(void)
{
    // disable stepper
    stepper.disable_stepper();
}

void Controller::dump_buffer(void)
{
    // print conntent of queue
    printk("Latest %d measurements:\n", BUFFER_LENGTH);
    for (unsigned i = 0; i < BUFFER_LENGTH; i++)
    {
        printk("%.3f, ", buffer.front());
        buffer.pop();
    }
    printk("\n");
}

void Controller::set_velocity(double vel)
{
    stepper.set_velocity(vel);
}

void Controller::set_reference(double ref)
{
    reference = ref;
}

// private
void Controller::control_loop_thread_entry(void *instance, void *, void *)
{
    auto thread_instance = reinterpret_cast<Controller *>(instance);
    thread_instance->control_loop_thread_function();
}

void Controller::control_loop_thread_function(void)
{
    while (1)
    {
        // wait for base_timer to expire
        // k_timer_status_sync(&control_loop_timer);;

        // measure stepper angle
        double phi_measured = stepper.get_position();
        buffer.pop();
        buffer.emplace(phi_measured);

        // update output
        error = reference - phi_measured;
        double gain = kp * error;
        stepper.set_velocity(gain);

        k_sleep(K_USEC(sample_time));
    }
}