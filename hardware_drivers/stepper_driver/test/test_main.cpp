#include <zephyr/ztest.h>
#include <zephyr/kernel.h>
#include "stepper.h"

ZTEST_SUITE(stepper_unit_tests, NULL, NULL, NULL, NULL, NULL);

/* create instance of stepper object only once for all the tests (C functions can't handle object destruction) */
Stepper stepper(4);

/* Logging*/
LOG_MODULE_REGISTER(unit_test_logger);

/* Order is random */
ZTEST(stepper_unit_tests, enable_disable_test)
{
	stepper.enable_stepper();
	k_sleep(K_MSEC(100));
	stepper.disable_stepper();
	ztest_test_pass();
}

ZTEST(stepper_unit_tests, disabled_steps_test)
{
	stepper.disable_stepper();
	k_sleep(K_MSEC(100));

	stepper.set_velocity(100.0);
	k_sleep(K_MSEC(100));

	stepper.set_velocity(0.0);
	zassert_true(stepper.get_position() < 0.5,
				 "\nSteps were generated while stepper was disabled");
}

ZTEST(stepper_unit_tests, various_speed_test)
{
	stepper.enable_stepper();
	k_sleep(K_MSEC(50));

	double slow_speed = 100.0;
	double high_speed = 600.0;

	// test one slow speed rotation
	stepper.set_velocity(slow_speed);
	k_sleep(K_MSEC(200000.0 / slow_speed));
	double first_pos = stepper.get_position();

	// test one high speed rotation
	stepper.set_velocity(high_speed);
	k_sleep(K_MSEC(200000.0 / high_speed));
	double second_pos = stepper.get_position();

	stepper.set_velocity(0);
	stepper.disable_stepper();

	// assert tests after execution! else failed tests will leave the stepper
	// in undefined states
	zassert_true((first_pos - 200) < 10,
				 "slow speed positioning not within 5%% error margin");
	zassert_true((second_pos - first_pos - 200) < 10,
				 "high speed positioning not within 5%% error margin");

	// log at the end to prevent timing issues
	LOG_MODULE_DECLARE(unit_test_logger);
	LOG_INF("first position: %0.3f", first_pos);
	LOG_INF("second position: %0.3f", second_pos - first_pos);
}

/* possible test functions */
// zassert_true(1, "1 was false");
// zassert_false(0, "0 was true");
// zassert_is_null(NULL, "NULL was not NULL");
// zassert_not_null("foo", "\"foo\" was NULL");
// zassert_equal(1, 1, "1 was not equal to 1");
// zassert_equal_ptr(NULL, NULL, "NULL was not equal to NULL");