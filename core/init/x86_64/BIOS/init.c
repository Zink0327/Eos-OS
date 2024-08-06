/*----------EOS kernel initial function for BIOS----------
    File name:BIOS/init.c

    Copyright (C) 2023 by Zink
    This file is part of EOS

    EOS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/
#include "init.h"

int init(kernelconfig *conf)
{
    svgamodinfostruct *modinfo = (svgamodinfostruct *)(__CORE_LINEAR_ADDR(0x8200));
    // screen
    conf -> screen -> fbaddr = (uint8_t *)(__CORE_LINEAR_ADDR(modinfo -> framebuffer));  // The address from 0xe0000000 was mapped to 0xffff800000a00000
    conf -> screen -> fblen = (uint32_t)(modinfo -> width * modinfo -> height * modinfo -> bpp / 8);

    conf -> screen -> x_res = modinfo -> width;
    conf -> screen -> y_res = modinfo -> height;
    conf -> screen -> bpp = modinfo -> bpp;

    conf -> screen -> x_size = 8;
    conf -> screen -> y_size = 16;
    //interrupt
    io_ltr(8); 	
    set_tss64(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00); 

    int_vector_init();
    return 0;
}

int init2(kernelconfig *conf)
{
	init_memory();
	return 0;
}
