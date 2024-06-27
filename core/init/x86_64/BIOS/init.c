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
    svgamodinfostruct *modinfo = (svgamodinfostruct *)0x8200;
    
    conf -> screen -> fbaddr = (unsigned int*)(modinfo -> framebuffer);
    conf -> screen -> fblen = (unsigned long)(modinfo -> width * modinfo -> height * modinfo -> bpp);

    conf -> screen -> x_res = (unsigned int)modinfo -> width;
    conf -> screen -> y_res = (unsigned int)modinfo -> height;

    conf -> screen -> x_size = 8;
    conf -> screen -> y_size = 16;
}
