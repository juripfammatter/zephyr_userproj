#include <stdio.h>
#include <stdbool.h>    // booleans
#include <zephyr/kernel.h>
#include <autoconf.h>
#include <vector>

int main(void)
{
    std::vector<int> vec(3,0);
    vec.push_back(42);

	printf("Hello World! %s\n", CONFIG_BOARD);
    int i = 0.0;
    while(true){
        printk("%s is working since %i seconds \n", CONFIG_BOARD, i);
        i++;
        k_msleep(1000);
    }
	return 0;
}

