/*
 *
 * included by io_device.h
 *
 */
#ifndef io_device_values_H_
#define io_device_values_H_

#ifdef IMPLEMENT_IO_DEVICE
//-----------------------------------------------------------------------------
//
// mplementtaion
//
//-----------------------------------------------------------------------------
#include <graphics/io_ssd1306.h>

def_constant_symbol(cr_START,			"start",5)


typedef struct PACK_STRUCTURE ssd1306_display {
	IO_MODAL_VALUE_STRUCT_PROPERTIES

	io_graphics_context_t *gfx;
	
} ssd1306_display_t;

static vref_t
ssd1306_display_start (io_t *io,vref_t r_ssd,vref_t const *args) {
	ssd1306_display_t *this = vref_cast_to_rw_pointer(r_ssd);
	
	if (this->gfx == NULL) {
		this->gfx = mk_ssd1306_io_graphics_context_twi (
			io,
			io_get_socket(io,OLED_SOCKET),
			OLED_FEATHER_LCDWIDTH,
			OLED_FEATHER_LCDHEIGHT,
			OLED_FEATHER_I2C_ADDRESS,
			8
		);
	}
	
	return cr_RESULT_CONTINUE;
}

EVENT_DATA io_signature_t ssd1306_normal_mode[] = {
	io_signature("","",ssd1306_display_start,arg_match(cr_START)),
	END_OF_MODE
};

static EVENT_DATA io_value_mode_t ssd1306_modes[] = {
	{"begin",ssd1306_normal_mode},
};

EVENT_DATA io_modal_value_implementation_t ssd1306_display_value_implementation = {
	decl_modal_value_implementation (io_modal_value_initialise,ssd1306_modes)
};

ssd1306_display_t cr_ssd1306_v = {
	decl_io_modal_value_m (
		&ssd1306_display_value_implementation,
		sizeof(ssd1306_display_t),
		ssd1306_modes
	)
	.gfx = NULL,
};

decl_particular_data_value(cr_SSD1306, ssd1306_display_t,cr_ssd1306_v)



#endif /* IMPLEMENT_IO_DEVICE */
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

