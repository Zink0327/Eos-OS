/*----------EOS memory processing module for BIOS----------
    File name:memory.c

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

#include "memory.h"

void init_memory()
{
    int32_t i;
    uint64_t total = 0;
    memstruct *p = (memstruct *)__CORE_LINEAR_ADDR(0x7e00);
    for (i = 0; i < 32; i++)
    {
        switch(p -> Type)
        {
            case 1:
            
                print(INDIGO, BLACK, "Memory block No.%d Length:%016d, Address: %#018x, Type: RAM(%05d)\n", i, p -> Length, p -> BaseAddr, p -> Type);
                total += p->Length;
                break;
            
            case 2:
            
                print(INDIGO, BLACK, "Memory block No.%d Length:%016d, Address: %#018x, Type: ROM/RESERVED(%05d)\n", i, p -> Length, p -> BaseAddr, p -> Type);
                break;
            
            case 3:
            
                print(INDIGO, BLACK, "Memory block No.%d Length:%016d, Address: %#018x, Type: ACPI Reclaim Memory(%05d)\n", i, p -> Length, p -> BaseAddr, p -> Type);
                break;
            
            case 4:
            
                print(INDIGO, BLACK, "Memory block No.%d Length:%016d, Address: %#018x, Type: ACPI NVS Memory(%05d)\n", i, p -> Length, p -> BaseAddr, p -> Type);
                break;
            
            default:
            
                print(INDIGO, BLACK, "Memory block No.%d Length:%016d, Address: %#018x, Type: Unknown(%05d)\n", i, p -> Length, p -> BaseAddr, p -> Type);
                break;
        };
         p++;
    }
    print(INDIGO, BLACK, "Total RAM memory: %#018lx", total);
    
}