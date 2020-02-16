/*
 *
 * pinky io device
 *
 *                        .--------------------.
 *                  Reset |16 0.18             |
 *                        |15 3v3              |
 *            (Button 1)  |14 0.11             |
 *                        |13 0v               |
 *                        |12 0.03     Vbat 12 | 
 *                        |11 0.04     Pen  11 | 
 *                        |10 0.28     Vusb 10 | 
 *                        | 9 0.29     1.03  9 | 
 *     WINC15x0 SPI_EN    | 8 0.30     1.12  8 | (Blue LED)
 *     WINC15x0 CS        | 7 0.31     1.11  7 | WINC15x0 Reset
 *     WINC15x0 SCLK      | 6 1.15     1.10  6 | WINC15x0 Enable
 *     WINC15x0 MOSI      | 5 1.13     1.08  5 | WINC15x0 Interrupt
 *     WINC15x0 MISO      | 4 1.14     1.02  4 | WINC15x0 Rx ->
 *     Console Uart Rx    | 3 0.08     1.01  3 | WINC15x0 Tx <-
 *     Console Uart Tx    | 2 0.06     0.27  2 | 
 *                        | 1 nc       0.26  1 | 
 *                        `--------------------'
 *
 *
 */
#ifndef io_device_H_
#define io_device_H_
#include <io_nrf52_winc15x0.h>
#include <io_board.h>

//
// allocate GPIOTE channels (0..7)
//
#define WINC1500_INTERRUPT_GPIOTE_CHANNEL		2

//
// allocate PPI channels (0..19)
//
#define TIME_CLOCK_PPI_CHANNEL					1

//
// binary pins
//
#define WINC1500_ENABLE			def_nrf_io_output_pin(1,10,NRF_GPIO_ACTIVE_LEVEL_HIGH,GPIO_PIN_INACTIVE)
#define WINC1500_RESET			def_nrf_io_output_pin(1,11,NRF_GPIO_ACTIVE_LEVEL_HIGH,GPIO_PIN_INACTIVE)
#define WINC1500_SPI_ENABLE	def_nrf_io_output_pin(0,30,NRF_GPIO_ACTIVE_LEVEL_HIGH,GPIO_PIN_ACTIVE)
#define WINC1500_INTERRUPT		def_nrf_io_interrupt_pin (\
											1,8,\
											NRF_IO_PIN_ACTIVE_LOW,\
											NRF_GPIO_PIN_PULLUP,\
											WINC1500_INTERRUPT_GPIOTE_CHANNEL,\
											GPIOTE_CONFIG_POLARITY_LoToHi\
										)

enum {
	USART0,
	USART1,
	SPI0,
	WINC15X0,
	
	NUMBER_OF_IO_SOCKETS // capture the socket count for this device
};

typedef struct PACK_STRUCTURE device_io_t {
	NRF52840_IO_CPU_STRUCT_MEMBERS

	io_socket_t* sockets[NUMBER_OF_IO_SOCKETS];
	
} device_io_t;

io_t*	initialise_device_io (void);
bool	test_device (io_t*,vref_t);

#ifdef IMPLEMENT_IO_DEVICE
//-----------------------------------------------------------------------------
//
// Implementation
//
//-----------------------------------------------------------------------------
#define UMM_VALUE_MEMORY_HEAP_SIZE		KB(4)
#define UMM_BYTE_MEMORY_SIZE				(KB(200) + UMM_VALUE_MEMORY_HEAP_SIZE)
#define UMM_SECTION_DESCRIPTOR      	__attribute__ ((section(".umm")))

static uint8_t ALLOCATE_ALIGN(8) UMM_SECTION_DESCRIPTOR
heap_byte_memory_bytes[UMM_BYTE_MEMORY_SIZE];
io_byte_memory_t
heap_byte_memory = {
	.heap = (umm_block_t*) heap_byte_memory_bytes,
	.number_of_blocks = (UMM_BYTE_MEMORY_SIZE / sizeof(umm_block_t)),
};

static io_value_memory_t* value_memories[1] = {0};
io_value_memory_t*
io_get_value_memory_by_id (uint32_t id) {
	if (id < SIZEOF(value_memories)) {
		return value_memories[id];
	} else {
		return NULL;
	}
}

bool
register_io_value_memory (io_value_memory_t *vm) {
	if (value_memories[io_value_memory_id(vm)] == NULL) {
		value_memories[io_value_memory_id(vm)] = vm;
		return true;
	} else {
		return false;
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

static nrf52_uart_t uart0 = {
	.implementation = &nrf52_uart_implementation,
	.encoding = IO_ENCODING_IMPLEMENATAION (&io_text_encoding_implementation),
	
	.uart_registers = NRF_UARTE0,
	.interrupt_number = UARTE0_UART0_IRQn,
	.baud_rate = UARTE_BAUDRATE_BAUDRATE_Baud115200,
	
	.tx_pin = def_nrf_gpio_alternate_pin(0,6),
	.rx_pin = def_nrf_gpio_alternate_pin(0,8),
	.rts_pin = def_nrf_gpio_null_pin(),
	.cts_pin = def_nrf_gpio_null_pin(),

};

static EVENT_DATA io_socket_constructor_t default_uart_constructor = {
	.encoding = NULL,
	.transmit_pipe_length = 5,
	.receive_pipe_length = 128,
};

static nrf52_uart_t uart1 = {
	.implementation = &nrf52_uart_implementation,
	.encoding = IO_ENCODING_IMPLEMENATAION (&io_text_encoding_implementation),
	
	.uart_registers = NRF_UARTE1,
	.interrupt_number = UARTE1_IRQn,
	.baud_rate = UARTE_BAUDRATE_BAUDRATE_Baud115200,
	
	.tx_pin = def_nrf_gpio_alternate_pin(1,2),
	.rx_pin = def_nrf_gpio_alternate_pin(1,1),
	.rts_pin = def_nrf_gpio_null_pin(),
	.cts_pin = def_nrf_gpio_null_pin(),

};

static nrf52_spi_t spi0 = {
	.implementation = &nrf52_spi_implementation,
	.encoding = NULL,
};

static nrf52_winc15x0_t winc1500 = {
	.implementation = &nrf52_winc15x0_implementation,
	
	.winc_enable_pin = WINC1500_ENABLE,
	.reset_pin = WINC1500_RESET,
	.interrupt_pin = WINC1500_INTERRUPT,
	.spi_enable_pin = WINC1500_SPI_ENABLE,
};

static EVENT_DATA io_socket_constructor_t winc1500_socket_constructor = {
	.encoding = NULL,
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

static device_io_t nrf_io = {
	.tc = {
		.high_timer = NRF_TIMER4,
		.low_timer = NRF_TIMER3,
		.interrupt_number = TIMER3_IRQn,
		.ppi_channel = TIME_CLOCK_PPI_CHANNEL,
	},
};

io_t*
initialise_device_io (void) {
	io_t *io = (io_t*) &nrf_io;
	
	add_io_implementation_device_methods (&io_i);

	initialise_io (io,&io_i);
	initialise_cpu_io (io);

	io_cpu_clock_start (io_get_core_clock(io));

	nrf_io.bm = initialise_io_byte_memory (io,&heap_byte_memory);
	nrf_io.vm = mk_umm_io_value_memory (io,UMM_VALUE_MEMORY_HEAP_SIZE,STVM);
	register_io_value_memory (nrf_io.vm);
	
	nrf_io.tasks = mk_io_value_pipe (nrf_io.bm,3);

	memset (nrf_io.sockets,0,sizeof(io_socket_t*) * NUMBER_OF_IO_SOCKETS);

	nrf_io.sockets[USART0] = io_socket_initialise (
		(io_socket_t*) &uart0,io,&default_uart_constructor
	);

	nrf_io.sockets[USART1] = io_socket_initialise (
		(io_socket_t*) &uart1,io,&default_uart_constructor
	);

	nrf_io.sockets[SPI0] = io_socket_initialise (
		(io_socket_t*) &spi0,io,NULL
	);
	
	nrf_io.sockets[WINC15X0] = io_socket_initialise (
		(io_socket_t*) &winc1500,io,&winc1500_socket_constructor
	);

	io_socket_open ((io_socket_t*) &uart0);

	// bind socket ..
	
	io_set_pin_to_output (io,LED1);
	

	return io;
}
#endif /* IMPLEMENT_IO_DEVICE */
#ifdef IMPLEMENT_VERIFY_IO_DEVICE
#include <verify_io.h>

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

void
io_device_unit_test (V_unit_test_t *unit) {
	static V_test_t const tests[] = {
		test_io_device_sockets_1,
		0
	};
	unit->name = "io device";
	unit->description = "io device unit test";
	unit->tests = tests;
	unit->setup = setup_io_device_unit_test;
	unit->teardown = teardown_io_device_unit_test;
}
#define IO_DEVICE_UNIT_TESTS	\
	nrf52_winc15x0_socket_unit_test,\
	io_device_unit_test,
	/**/
#else
# define IO_DEVICE_UNIT_TESTS
#endif /* IMPLEMENT_VERIFY_IO_DEVICE */
#endif /* IMPLEMENT_IO_DEVICE */
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
