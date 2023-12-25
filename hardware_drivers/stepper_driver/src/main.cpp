#include "stepper.h"
#include "controller.h"

int main(void)
{
	// Stepper stepper(4); // using step divider 1/4
	// k_sleep(K_MSEC(20));
	// stepper.enable_stepper();

	// stepper.set_velocity(600.0); // 3 rotations per second
	// k_sleep(K_MSEC(333));		 // one full rotation
	// printk("Steps: %f\n", stepper.get_position());

	// stepper.set_velocity(-600.0); // -3 rotations per second
	// k_sleep(K_MSEC(333));		  // one full rotation
	// printk("Steps: %f\n", stepper.get_position());

	// // deconstructor will disable Stepper

	Controller controller(4, 5000); // kp, sample time (usec)

	// controller.set_velocity(20);
	// k_sleep(K_MSEC(2000));

	controller.set_reference(200);
	k_sleep(K_MSEC(1500));

	controller.dump_buffer();
	k_sleep(K_MSEC(100));

	return 0;
}
