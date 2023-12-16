#include <stdio.h>
#include <stdbool.h>    // booleans
#include <zephyr/kernel.h>
#include <autoconf.h>

/* Thread */
#define PWM_STACK_SIZE 500
#define THREAD_A_PRIORITY 5

K_THREAD_STACK_DEFINE(pwm_thread_stack_area, PWM_STACK_SIZE);
struct k_thread pwm_thread;

// Thread function declaration
void pwm_thread_function(void *p1, void *p2, void *p3);

/* Timer */
float duty_cycle = 0.3;     // percentage on
unsigned period = 2000;     //ms
unsigned resolution = 200;  // ms

struct k_timer base_timer;
struct k_timer period_timer;
void expiry_function(struct k_timer *timer_id);

K_TIMER_DEFINE(base_timer, expiry_function, NULL);
K_TIMER_DEFINE(period_timer, NULL, NULL);

int main(void)
{
    /* Initialize Thread */
    k_tid_t thread_a_tid = k_thread_create(&pwm_thread, pwm_thread_stack_area,
                                K_THREAD_STACK_SIZEOF(pwm_thread_stack_area),
                                pwm_thread_function,
                                NULL, NULL, NULL,
                                THREAD_A_PRIORITY, 0, K_FOREVER);
    k_thread_name_set(&pwm_thread, "pwm_thread");                    
    k_thread_start(&pwm_thread); // start after naming thread

    /* Initialize base_timer*/
    printk("starting timers\n");
    k_timer_start(&base_timer, K_NO_WAIT, K_MSEC(resolution));
    k_timer_start(&period_timer, K_NO_WAIT, K_MSEC(period));

    // wait for base_timer to expire
    k_timer_status_sync(&base_timer);
    /* check base_timer status */
    printk("base_timer status: %i\n", k_timer_status_get(&base_timer));
    printk("time remaining: %i\n", k_timer_remaining_get(&base_timer));
    printk("systicks remaining: %i\n", (int) k_timer_remaining_ticks(&base_timer));
    printk("expiration time: %i\n", (int) k_timer_expires_ticks(&base_timer));

	return 0;
}

/* expiration function resumes suspended thread */
void expiry_function(struct k_timer *timer_id){
    k_thread_resume(&pwm_thread);
}

/* thread generates pwm signal (downcounting) */
void pwm_thread_function(void *p1, void *p2, void *p3){
    while(1){
        k_thread_suspend(&pwm_thread);
        if(k_timer_remaining_get(&period_timer) > duty_cycle * ((float) period)){
            printk(".");
        } else {
            printk("|");
        }
        
        
    }
}