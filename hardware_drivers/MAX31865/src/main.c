#include <stdio.h>
//#include <stdbool.h>    // booleans
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/spi.h>
#include <autoconf.h>

int main(void)
{
	printf("%s running\n", CONFIG_BOARD);

	const struct device *dev_0 = DEVICE_DT_GET(DT_NODELABEL(max31865_0));
	const struct device *dev_1 = DEVICE_DT_GET(DT_NODELABEL(max31865_1));

	if (!device_is_ready(dev_0)) {
		printk("MAX31865 device 0 not ready\n");
		return 0;
	}
	if (!device_is_ready(dev_1)) {
		printk("MAX31865 device 1 not ready\n");
		return 0;
	}

	printk("Device %p name is %s\n", dev_0, dev_0->name);
	printk("Device %p name is %s\n", dev_1, dev_1->name);

	while (1) {
		k_sleep(K_MSEC(1000));
		struct sensor_value temp_0;
		struct sensor_value temp_1;

		sensor_sample_fetch(dev_0);
		sensor_channel_get(dev_0, SENSOR_CHAN_AMBIENT_TEMP, &temp_0);

		sensor_sample_fetch(dev_1);
		sensor_channel_get(dev_1, SENSOR_CHAN_AMBIENT_TEMP, &temp_1);

		printk("Temperature sensor 0: %.2f C\n", sensor_value_to_double(&temp_0));
		printk("Temperature sensor 1: %.2f C\n", sensor_value_to_double(&temp_1));
		printf("---\n");

	}
	return 0;
}

