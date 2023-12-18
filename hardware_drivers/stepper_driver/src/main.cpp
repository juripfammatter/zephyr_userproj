#include "stepper.h"

int main(void)
{
	Stepper stepper(4); // using step divider 1/4
	k_sleep(K_SECONDS(2));
	stepper.enable_stepper();

	stepper.set_velocity(200.0);
	k_sleep(K_MSEC(1000));
	printk("Steps: %f\n", stepper.get_position());

	stepper.set_velocity(-200.0);
	k_sleep(K_MSEC(1000));
	printk("Steps: %f\n", stepper.get_position());

	stepper.disable_stepper();
	// while(1);

	return 0;
}
