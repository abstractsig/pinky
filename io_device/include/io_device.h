/*
 *
 * pinky io device
 *
 */
#ifndef io_device_H_
#define io_device_H_
#include <io_board.h>

#define UMM_GLOBAL_HEAP_SIZE				0xc000
#define UMM_VALUE_HEAP_SIZE				0x1000
#define UMM_SECTION_DESCRIPTOR      	__attribute__ ((section(".umm")))


enum {
	CONSOLE_SOCKET,

	NUMBER_OF_IO_SOCKETS // capture the socket count for this device
};


typedef struct PACK_STRUCTURE device_io_t {
	NRF52840_IO_CPU_STRUCT_MEMBERS

	io_socket_t* sockets[NUMBER_OF_IO_SOCKETS];
	
} device_io_t;

io_t*	initialise_device_io (void);
bool	test_device (io_t*,vref_t);

#ifdef IMPLEMENT_IO_DEVICE

static uint8_t ALLOCATE_ALIGN(8) UMM_SECTION_DESCRIPTOR
heap_byte_memory_bytes[UMM_GLOBAL_HEAP_SIZE];
io_byte_memory_t
heap_byte_memory = {
	.heap = (umm_block_t*) heap_byte_memory_bytes,
	.number_of_blocks = (UMM_GLOBAL_HEAP_SIZE / sizeof(umm_block_t)),
};

static uint8_t ALLOCATE_ALIGN(8) UMM_SECTION_DESCRIPTOR
stvm_byte_memory_bytes[UMM_VALUE_HEAP_SIZE];
static io_byte_memory_t
stvm_byte_memory = {
	.heap = (umm_block_t*) stvm_byte_memory_bytes,
	.number_of_blocks = (UMM_VALUE_HEAP_SIZE / sizeof(umm_block_t)),
};

umm_io_value_memory_t short_term_values = {
	.implementation = &umm_value_memory_implementation,
	.id_ = STVM,
	.bm = &stvm_byte_memory,
};

//
// success registered value memories
//
io_value_memory_t*
io_get_value_memory_by_id (uint32_t id) {
	if (id == STVM) {
		return (io_value_memory_t*) &short_term_values;
	} else {
		return NULL;
	}
}

static io_cpu_clock_pointer_t
nrf_get_core_clock (io_t *io) {
	extern EVENT_DATA nrf52_core_clock_t cpu_core_clock;
	return IO_CPU_CLOCK(&cpu_core_clock);
}

static io_socket_t*
io_device_get_socket (io_t *io,int32_t handle) {
	if (handle >= 0 && handle < NUMBER_OF_IO_SOCKETS) {
		device_io_t *this = (device_io_t*) io;
		return this->sockets[handle];
	} else {
		return NULL;
	}
}

/*
 *-----------------------------------------------------------------------------
 *
 * CPU Clock Tree
 *
 *    +----------+                      64MHz   +--------+
 *    | XTAL/1   |-+--------------------------->| CPU    |		          
 *    +----------+ |                            +--------+
 *                 |
 *                 |       +---------+          +---------+
 *                 +------>| APB0    |--------->| UART1   |		// console uart
 *                 |       +---------+   |      +---------+
 *                 |                     |
 *                 |                     |      +---------+
 *                 |                     |----->| RTC     |		// time clock
 *                 |                     |      +---------+
 *
 *-----------------------------------------------------------------------------
 */
extern EVENT_DATA nrf52_core_clock_t cpu_core_clock;

EVENT_DATA nrf52_oscillator_t crystal_oscillator = {
	.implementation = &nrf52_crystal_oscillator_implementation,
	.outputs = (const io_cpu_clock_pointer_t []) {
		decl_io_cpu_clock_pointer(&cpu_core_clock),
		{NULL}
	},
};

EVENT_DATA nrf52_core_clock_t cpu_core_clock = {
	.implementation = &nrf52_core_clock_implementation,
	.input = IO_CPU_CLOCK(&crystal_oscillator),
};

nrf52_uart_t console_uart = {
	.implementation = &nrf52_uart_implementation,
	.encoding = IO_ENCODING_IMPLEMENATAION (&io_text_encoding_implementation),
	
	.uart_registers = NRF_UARTE1,
	.interrupt_number = UARTE1_IRQn,
	.baud_rate = UARTE_BAUDRATE_BAUDRATE_Baud115200,
	
	.tx_pin = def_nrf_gpio_alternate_pin(0,6),
	.rx_pin = def_nrf_gpio_alternate_pin(0,8),
	.rts_pin = def_nrf_gpio_null_pin(),
	.cts_pin = def_nrf_gpio_null_pin(),

};

EVENT_DATA io_socket_constructor_t console_uart_constructor = {
	.transmit_pipe_length = 5,
	.receive_pipe_length = 128,
};

void
add_io_implementation_device_methods (io_implementation_t *io_i) {
	add_io_implementation_board_methods (io_i);

	io_i->get_core_clock = nrf_get_core_clock;
	io_i->get_socket = io_device_get_socket;
}

static io_implementation_t io_i = {
	0
};

static device_io_t nrf_io = {0};

io_t*
initialise_device_io (void) {
	io_t *io = (io_t*) &nrf_io;
	
	add_io_implementation_device_methods (&io_i);

	initialise_io (io,&io_i);
	initialise_cpu_io (io);

	io_cpu_clock_start (io_get_core_clock(io));

	nrf_io.bm = &heap_byte_memory;
	nrf_io.vm = (io_value_memory_t*) &short_term_values;

	short_term_values.io = io;
	initialise_io_byte_memory (io,&heap_byte_memory);
	initialise_io_byte_memory (io,&stvm_byte_memory);
	
	memset (nrf_io.sockets,0,sizeof(io_socket_t*) * NUMBER_OF_IO_SOCKETS);

	io_socket_initialise (
		(io_socket_t*) &console_uart,io,&console_uart_constructor
	);
	nrf_io.sockets[CONSOLE_SOCKET] = (io_socket_t*) &console_uart;
	io_socket_open ((io_socket_t*) &console_uart);

	io_set_pin_to_output (io,LED1);
	

	return io;
}
#endif /* IMPLEMENT_IO_DEVICE */
#ifdef IMPLEMENT_VERIFY_IO_DEVICE
#include <verify_io.h>

#endif /* IMPLEMENT_VERIFY_IO_DEVICE */
#ifdef IMPLEMENT_VERIFY_IO_CORE
UNIT_SETUP(setup_io_device_unit_test) {
	return VERIFY_UNIT_CONTINUE;
}

UNIT_TEARDOWN(teardown_io_device_unit_test) {
}

void
io_device_unit_test (V_unit_test_t *unit) {
	static V_test_t const tests[] = {
		#ifdef IMPLEMENT_VERIFY_IO_DEVICE
		#endif
		0
	};
	unit->name = "io device";
	unit->description = "io device unit test";
	unit->tests = tests;
	unit->setup = setup_io_device_unit_test;
	unit->teardown = teardown_io_device_unit_test;
}
#endif /* IMPLEMENT_VERIFY_IO_CORE */
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
