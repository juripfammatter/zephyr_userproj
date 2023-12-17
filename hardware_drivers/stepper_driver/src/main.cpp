#include "stepper.h"

int main(void)
{
	Stepper stepper(4);
	k_sleep(K_SECONDS(2));
	stepper.enable_stepper();

	stepper.set_velocity(20000.0);
	k_sleep(K_MSEC(1000));
	stepper.set_velocity(-20000.0);
	k_sleep(K_MSEC(1000));

	stepper.disable_stepper();
	// while(1);

	return 0;
}
