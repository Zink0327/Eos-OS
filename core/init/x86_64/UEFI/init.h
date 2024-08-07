/*----------EOS kernel initial function for UEFI----------
    File name:UEFI/init.h

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

#ifndef _X86_64_UEFI_INIT
#define _X86_64_UEFI_INIT

struct _scrnconfig {
 /* resolution */
	uint32_t x_res;
	uint32_t y_res;
    uint8_t bpp;
	
	/* size */
	uint32_t x_size;
	uint32_t y_size;
 /* frame buffer and length */
    uint32_t *fbaddr;
    uint64_t fblen;

};

typedef struct _scrnconfig scrnconfig;

struct _KernelConfig {
    scrnconfig *screen;

 
};

typedef struct _KernelConfig kernelconfig;


int init(kernelconfig *conf); /* main function */

#endif
