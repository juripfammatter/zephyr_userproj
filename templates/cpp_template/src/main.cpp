#include <cstdio>
#include <zephyr/kernel.h>
#include <autoconf.h>
#include <vector>

int main(void)
{
    std::vector<int> vec = {0, 1, 2};
    vec.push_back(42);

    for(auto e: vec){
        printk("%i\n", e);
    }
 
	printf("Hello World! %s\n", CONFIG_BOARD);
    int i = 0.0;
    while(true){
        printk("%s is working since %i seconds \n", CONFIG_BOARD, i);
        i++;
        k_msleep(1000);
    }
	return 0;
}

