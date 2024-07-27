#include <stdio.h>
//#include <stdbool.h>    // booleans
#include <zephyr/kernel.h>
#include <autoconf.h>

int main(void)
{
	printf("Hello World! %s\n", CONFIG_BOARD);
    int i = 0;
    while(i<10){
        printk("%s is working since %i seconds \n", CONFIG_BOARD, i);
        i++;
        k_msleep(1000);
    }
	return 0;
}
