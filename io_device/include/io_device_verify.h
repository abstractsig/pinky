/*
 *
 *
 */
#ifndef io_device_verify_H_
#define io_device_verify_H_
#include <io_verify.h>

void	run_ut_io_device (V_runner_t*);

#ifdef IMPLEMENT_VERIFY_IO_DEVICE
#include <io_device.h>
#include <io_verify.h>
#include <nrf52_qspi_verify.h>

TEST_BEGIN(test_io_twi_master_socket_1) {
	io_socket_t *twi = io_get_socket (TEST_IO,TWIM0_SOCKET);
	
	if (VERIFY (twi != NULL,NULL)) {
		io_socket_open (twi,IO_SOCKET_OPEN_CONNECT);
		
		io_socket_close (twi);
	}
}
TEST_END

TEST_BEGIN(test_io_device_sockets_1) {
	VERIFY (io_get_socket (TEST_IO,USART0) != NULL,NULL);
	VERIFY (io_get_socket (TEST_IO,USART1) != NULL,NULL);
	VERIFY (io_get_socket (TEST_IO,SPI0) != NULL,NULL);
}
TEST_END

UNIT_SETUP(setup_io_device_unit_test) {
	return VERIFY_UNIT_CONTINUE;
}

UNIT_TEARDOWN(teardown_io_device_unit_test) {
}

static void
io_device_unit_test (V_unit_test_t *unit) {
	static V_test_t const tests[] = {
		test_io_device_sockets_1,
		test_io_twi_master_socket_1,
		0
	};
	unit->name = "io device";
	unit->description = "io device unit test";
	unit->tests = tests;
	unit->setup = setup_io_device_unit_test;
	unit->teardown = teardown_io_device_unit_test;
}

void
run_ut_io_device (V_runner_t *runner) {
	static const unit_test_t test_set[] = {
		io_device_unit_test,
		0
	};
	V_run_unit_tests(runner,test_set);
}

#endif /* IMPLEMENT_VERIFY_IO_DEVICE */
#endif
/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2020 Gregor Bruce
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
