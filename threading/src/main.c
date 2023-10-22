#include <stdio.h>
#include <stdbool.h>    // booleans
#include <zephyr/kernel.h>

// Threads
#define MY_STACK_SIZE 500
#define THREAD_A_PRIORITY 5
#define THREAD_B_PRIORITY 5

K_THREAD_STACK_DEFINE(thread_a_stack_area, MY_STACK_SIZE);
struct k_thread thread_a;

K_THREAD_STACK_DEFINE(thread_b_stack_area, MY_STACK_SIZE);
struct k_thread thread_b;

// Thread function
void thread_a_function(void *p1, void *p2, void *p3);
void thread_b_function(void *p1, void *p2, void *p3);

int thread_a_counter = 0;

int main(void)
{
    printf("%s started.\n", CONFIG_BOARD);

    // create threads
    k_tid_t thread_a_tid = k_thread_create(&thread_a, thread_a_stack_area,
                                 K_THREAD_STACK_SIZEOF(thread_a_stack_area),
                                 thread_a_function,
                                 NULL, NULL, NULL,
                                 THREAD_A_PRIORITY, 0, K_NO_WAIT);
    k_thread_name_set(&thread_a, "thread_a");                    
    // k_thread_start(&thread_a); // not necessary with K_NO_WAIT

    k_tid_t thread_b_tid = k_thread_create(&thread_b, thread_b_stack_area,
                                 K_THREAD_STACK_SIZEOF(thread_b_stack_area),
                                 thread_b_function,
                                 NULL, NULL, NULL,
                                 THREAD_B_PRIORITY, 0, K_MSEC(1000));
    k_thread_name_set(&thread_b, "thread_b");                    
    // k_thread_start(&thread_b); // not necessary with K_MSEC(1000)

    printf("Threads created.\n");
	return 0;
}

/* thread_a waits for thread_b, then loops 10 times and aborts */
void thread_a_function(void *p1, void *p2, void *p3){
    printk("thread a: started\n");
    printk("thread a: waiting for thread b\n");
    k_thread_join(&thread_b, K_FOREVER);            // wait for thread b to abort

    while(1){
        printk("thread a: running %x more times\n", 10-thread_a_counter);
        k_msleep(500); 

        if (thread_a_counter++ >= 10){
            printk("thread a aborting\n");
            k_thread_abort(&thread_a);
        }
        
    }
}

/* thread_b waits for 3s then aborts */
void thread_b_function(void *p1, void *p2, void *p3){
    printk("thread b: started\n");
    k_msleep(3000);
    printk("thread b: finished\n");
    k_thread_abort(&thread_b); // not necessary
}
