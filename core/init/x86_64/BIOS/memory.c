/*----------EOS memory processing module for BIOS----------
    File name:memory.c

    Copyright (C) 2024 by Zink
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

void init_memory(memconfig *conf)
{
    uint64_t total = 0;

    conf -> counts = 0;
    memstruct *p = (memstruct *)__CORE_LINEAR_ADDR(0x7e00);
    uint16_t count = 0;
    for (int16_t i = 0; i < 32; i++)
    {
        if(p->Type > 4 || p->Type <= 0)
            break;
        
        conf->blocks[count].addr = p -> BaseAddr;
        conf->blocks[count].type = p -> Type;
        switch(p -> Type)
        {
            case 1:
            
                print(INDIGO, BLACK, "Memory block No.%d Length:%016d, Address: %#018x, Type: RAM(%05d)\n", i, p -> Length, p -> BaseAddr, p -> Type);
                
                conf->blocks[count].size = 2048;  //the memory page is 2M aligned

                /* count the memory block's length */
                uint64_t end = PAGE_2M_SHL(PAGE_2M_SHR(conf->blocks[count].addr + p -> Length));
                uint64_t start = PAGE_2M_ALIGN(conf->blocks[count].addr);

                if (end > start)
                {  /* if the length is valid, record it. */
                    conf->blocks[count].len = PAGE_2M_SHR(end - start);
                    total += PAGE_2M_SHR(end - start);
                }
                else
                {  /* Or just ignore the memory block... */
                    count--;
                    conf->counts--;
                }
                
                break;
            
            case 2:
            
                print(INDIGO, BLACK, "Memory block No.%d Length:%016d, Address: %#018x, Type: ROM/RESERVED(%05d)\n", i, p -> Length, p -> BaseAddr, p -> Type);
                conf->blocks[count].len = (p->Length / 1024);
                conf->blocks[count].size = 1;
                break;
            
            case 3:
            
                print(INDIGO, BLACK, "Memory block No.%d Length:%016d, Address: %#018x, Type: ACPI Reclaim Memory(%05d)\n", i, p -> Length, p -> BaseAddr, p -> Type);
                conf->blocks[count].len = (p->Length / 1024);
                conf->blocks[count].size = 1;
                break;
            
            case 4:
            
                print(INDIGO, BLACK, "Memory block No.%d Length:%016d, Address: %#018x, Type: ACPI NVS Memory(%05d)\n", i, p -> Length, p -> BaseAddr, p -> Type);
                conf->blocks[count].len = (p->Length / 1024);
                conf->blocks[count].size = 1;
                break;

        };

        count++;
        conf->counts++;
        p++;
    }
    print(INDIGO, BLACK, "Total Available Page Count: %#d", total);
    
}