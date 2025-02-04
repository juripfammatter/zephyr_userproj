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

	const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(max31865));
	if (!device_is_ready(dev)) {
		printk("MAX31865 device not ready\n");
		return 0;
	}

	printk("Device %p name is %s\n", dev, dev->name);

	while (1) {
		k_sleep(K_MSEC(1000));
		struct sensor_value temp;

		sensor_sample_fetch(dev);
		sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
		printk("Temperature: %.2f C\n", sensor_value_to_double(&temp));

	}
	return 0;
}

