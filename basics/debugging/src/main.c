#include <stdio.h>
#include <stdbool.h>    // booleans
#include <zephyr/kernel.h>
#include <autoconf.h>
#include <zephyr/logging/log.h>

// Register logging module
LOG_MODULE_REGISTER(test);

int main(void)
{
    // attach to logging
    LOG_MODULE_DECLARE(test);
    LOG_DBG("Debug level logging"); // Level 4
    LOG_INF("Info level logging"); // Level 3
    LOG_WRN("Warning level logging"); // Level 2
    LOG_ERR("Error level logging"); // Level 1
    k_msleep(1000);

	printf("Hello World! %s\n", CONFIG_BOARD);
    int i = 0.0;
    while(true){
        printk("%s is working since %i seconds \n", CONFIG_BOARD, i);
        i++;
        k_msleep(1000);
    }
	return 0;
}
