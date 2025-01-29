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

void putchar_framebuffer(uint8_t* fb, uint8_t bpp, uint16_t width, int x, int y, uint32_t FRcolor, uint32_t BKcolor, uint8_t fontn)
{
	if (fb == NULL || bpp == 0 || (bpp % 8) != 0)
		return;

	int i = 0, j = 0, testval = 0, btpp = bpp / 8, counter = 0;
	uint8_t* addr = NULL, * fontp = font[fontn], inbox = 0;

	for (i = 0; i < 16; i++)
	{
		addr = (uint8_t*)(fb + (width * (y + i) + x) * btpp);
		testval = 0x100;
		for (j = 0; j < 8; j++)
		{
			testval = testval >> 1;
			if (*fontp & testval)
			{
				while (counter < btpp)
				{
					inbox = (uint8_t)(FRcolor >> (8 * counter)) & 0x000000FF;
					*addr = inbox;
					++addr;
					counter++;
				}
			}
			else
			{
				while (counter < btpp)
				{
					inbox = (uint8_t)(BKcolor >> (8 * counter)) & 0x000000FF;
					*addr = inbox;
					++addr;
					counter++;
				}
			}
			counter = 0;
		}
		fontp++;
	}

}

int init(kernelconfig *conf)
{
    svgamodinfostruct *modinfo = (svgamodinfostruct *)(__CORE_LINEAR_ADDR(0x8200));
    // screen
    conf -> screen . fbaddr = (uint8_t *)(__CORE_LINEAR_ADDR(modinfo -> framebuffer));
    conf -> screen . fblen = (uint32_t)(modinfo -> width * modinfo -> height * modinfo -> bpp / 8);

    conf -> screen . x_res = modinfo -> width;
    conf -> screen . y_res = modinfo -> height;
    conf -> screen . bpp = modinfo -> bpp;

    conf -> screen . x_size = 8;
    conf -> screen . y_size = 16;
    conf -> screen . putchar_k = putchar_framebuffer;
    //interrupt
    io_ltr(8); 	
    set_tss64(0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00, 0xffff800000007c00); 

    int_vector_init();
    return 0;
}

int init2(kernelconfig *conf)
{
    for (uint16_t i = 0; i < 32; ++i)
    {
        conf->memory.blocks[i].addr = 0;
        conf->memory.blocks[i].len = 0;
        conf->memory.blocks[i].type = 0;
        conf->memory.blocks[i].size = 0;
    }
    
	init_memory(&conf->memory);
        
    print(WHITE, BLACK, "\nFetched Memory Block: Address               Length              Type              Size(Kbyte) \n");
    for (uint16_t i = 0; i < conf->memory.counts; i++)
    {
        print(WHITE, BLACK, "Memory Block No.%d, %#018x, %#018d, %#018d, %#018d\n", i, conf->memory.blocks[i].addr, conf->memory.blocks[i].len, conf->memory.blocks[i].type, conf->memory.blocks[i].size);
    }
    
	return 0;
}
