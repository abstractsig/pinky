/*
 *
 * the wifi pinky starts here
 *
 */
#define IMPLEMENT_IO_CORE
#define IMPLEMENT_IO_CPU
#define IMPLEMENT_IO_TLS
#define IMPLEMENT_IO_DEVICE
#define IMPLEMENT_IO_BOARD
#define IMPLEMENT_IO_TLS
#include <io_device.h>

int
main (void) {
	io_t *io = initialise_device_io ();
	bool first_run = io_is_first_run (io);
	
	if (first_run) {
	}

	if (test_device (io,cr_NIL)) {
		while (1) {
			io_wait_for_event (io);
			io_do_gc (io,-1);
		}
	} else {
		io_printf(io,"\n"DEVICE_NAME" device test failed\n");
	}

	return 0;
}
