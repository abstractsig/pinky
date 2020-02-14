/*
 *
 * verify device functionality
 *
 */
#define IMPLEMENT_VERIFY_IO_CORE
#define IMPLEMENT_VERIFY_IO_CORE_VALUES
#define IMPLEMENT_VERIFY_IO_CPU
#define IMPLEMENT_VERIFY_IO_DEVICE
#include <verify_io.h>

/*
 *-----------------------------------------------------------------------------
 *
 * test_device --
 *
 *-----------------------------------------------------------------------------
 */
bool
test_device (io_t *io,vref_t r_led) {
	V_runner_t runner = {
		.user_value = r_led,
		.io = io,
	};

	V_start_tests(&runner);
	verify_io (&runner);
	print_unit_test_report (&runner);

	return runner.total_failed == 0;
}
