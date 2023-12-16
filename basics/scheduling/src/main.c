#include <stdio.h>
#include <stdbool.h>    // booleans
/* Zephyr includes */
#include <zephyr/kernel.h>
#include <autoconf.h>

// Threads
#define MY_STACK_SIZE 1024

#define THREAD_A_PRIORITY (1)      // cooperative time slicing
#define THREAD_B_PRIORITY (1)      // cooperative time slicing (higher prio)

K_THREAD_STACK_DEFINE(thread_a_stack_area, MY_STACK_SIZE);
struct k_thread thread_a_data;

K_THREAD_STACK_DEFINE(thread_b_stack_area, MY_STACK_SIZE);
struct k_thread thread_b_data;

// Thread functions
void thread_a_function(void *p1, void *p2, void *p3);
void thread_b_function(void *p1, void *p2, void *p3);
void print_runtime_stats(k_tid_t tid);

int main(void)
{
	
    printf("%s started.\n", CONFIG_BOARD);

    // create threads and start immediately
     
    k_tid_t thread_a_tid = k_thread_create(&thread_a_data, thread_a_stack_area,
                                 K_THREAD_STACK_SIZEOF(thread_a_stack_area),
                                 thread_a_function,
                                 NULL, NULL, NULL,
                                 THREAD_A_PRIORITY, 0, K_FOREVER);
    k_thread_name_set(&thread_a_data, "thread_a"); 
    k_thread_start(&thread_a_data);

    // starts with delay
    k_tid_t thread_b_tid = k_thread_create(&thread_b_data, thread_b_stack_area,
                                 K_THREAD_STACK_SIZEOF(thread_b_stack_area),
                                 thread_b_function,
                                 NULL, NULL, NULL,
                                 THREAD_B_PRIORITY, 0, K_MSEC(50));
    k_thread_name_set(&thread_b_data, "thread_b");                    
    //k_thread_start(&thread_b_data);

	return 0;
}


// Thread A 
void thread_a_function(void *p1, void *p2, void *p3){
    const char* thread_name = k_thread_name_get(k_current_get());
    int thread_priority = k_thread_priority_get(k_current_get());

    printk("%s: started with priority: %d\n", thread_name, thread_priority);
     for(int i = 0; i<5; i++){
        printk("%s: running and counting: %d\n",thread_name, i);
        k_busy_wait(50000); // doesn't yields
     }
    
    print_runtime_stats(k_current_get());
    
}

// Thread B
void thread_b_function(void *p1, void *p2, void *p3){
    const char* thread_name = k_thread_name_get(k_current_get());
    int thread_priority = k_thread_priority_get(k_current_get());

    printk("%s: started with priority: %d\n", thread_name, thread_priority);
     for(int i = 0; i<5; i++){
        printk("%s: running and counting: %d\n",thread_name, i);
        k_busy_wait(50000); // doesn't yields
     }

     print_runtime_stats(k_current_get());
}

void print_runtime_stats(k_tid_t tid){
    k_thread_runtime_stats_t rt_stats_thread;
    const char* thread_name = k_thread_name_get(k_current_get());

    k_thread_runtime_stats_get(tid, &rt_stats_thread);
    printk("%s ran %llu cycles \n",thread_name ,rt_stats_thread.execution_cycles);
}