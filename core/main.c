/*----------EOS core entry----------
    File name:main.c

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

#include "main.h"

void SystemCollapsed()
{
    // 
    while(1);
}


int Start_Kernel(kernelconfig config)
{
    kernelconfig osconf;

    //  init everything if necessary
    init(&osconf);  //defined at ./global.h
    backupscrn.fbaddr = (uint8_t *)(osconf.screen->fbaddr + 0xffff800000000000);
    backupscrn.fblen = osconf.screen->fblen;
    backupscrn.x_res = osconf.screen->x_res;
    backupscrn.y_res = osconf.screen->y_res;
    backupscrn.x_size = osconf.screen->x_size;
    backupscrn.y_size = osconf.screen->y_size;

    print(WHITE,BLACK,"hello world!");

    //  load and start the "real" kernel
    

    //  receive interrupt from EIH
    while(1)
        ;
    
    //  it isn't supposed to be here
    SystemCollapsed();
}
