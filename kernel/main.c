/*----------Eos kernel entry----------
    File name:main.c

    Copyright (C) 2023 by Zink
    This file is part of Eos

    Eos is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Eos is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "main.h"

void SystemPanic()
{
    // 
    while(1);
}


int Start_Kernel(KernelConfig config)
{
    //  init everything if necessary
    BIOSinit();   //at ./init/BIOS/BIOSinit.c
    print(WHITE,BLACK,)
    //  start the first process
    

    //  loop over and over again
    while(1)
        ;
    
    //  it isn't supposed to be here
    SystemPanic();
}