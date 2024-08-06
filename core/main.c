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




int Start_Kernel(kernelconfig *config)
{
    kernelconfig osconf;

    //  init everything if necessary
    init(&osconf);  //defined at ./global.h
    print_init(backupscrn,osconf)

    long i=0xABCDEF;

    print(WHITE,BLACK,"Hello world\nabc %064lb,%#018lx \n",i,i);
    init2(&osconf);
    //  load and start the "real" kernel
    

    //  receive interrupt from EIH
    while(1)
        ;
    
    //  it isn't supposed to be here
    SystemCollapsed();
}
