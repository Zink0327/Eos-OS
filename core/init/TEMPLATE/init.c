/*----------EOS kernel initial function for xxx----------
    File name:init.c

    Copyright (C) 20xx by xxx
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
// this file is the template of the initialize part of the EOS core.

#include "init.h"

int init(kernelconfig *conf)
{
    // screen
    conf -> screen . fbaddr = xxx
    conf -> screen . fblen = xxx

    conf -> screen . x_res = xxx;
    conf -> screen . y_res = xxx;
    conf -> screen . bpp = xxx;

    conf -> screen . x_size = xx;
    conf -> screen . y_size = xx;
    //interrupt
    
    xxx

    return 0;
}

int init2(kernelconfig *conf)
{
    //clear the struct first
    for (uint16_t i = 0; i < xx; ++i)
    {
        conf->memory.blocks[i].xxx = 0;
    }
    
	init_memory(&conf->memory);
        
    print(WHITE, BLACK, "\nFetched Memory Block: Address               Length              Type              Size(Kbyte) \n");
    for (uint16_t i = 0; i < conf->memory.counts; i++)
    {
        xxx
    }

    //others
    xxx
	return 0;
}
