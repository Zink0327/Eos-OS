/*----------EOS kernel initial function for BIOS----------
    File name:BIOS/init.h

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

#ifndef _X86_64_BIOS_INIT
#define _X86_64_BIOS_INIT

#include "vbe.h"

struct _scrnconfig {
 /* resolution */
	uint16_t x_res;
	uint16_t y_res;
    uint8_t bpp;
	
	/* size */
	uint16_t x_size;
	uint16_t y_size;
 /* frame buffer and length */
    uint8_t *fbaddr;
    uint32_t fblen;

};

typedef struct _scrnconfig scrnconfig;

struct _KernelConfig {
    scrnconfig *screen;

 
};

typedef struct _KernelConfig kernelconfig;


int init(kernelconfig *conf); /* main function */

#endif
