/*----------EOS memory processing module for xxx----------
    File name:memory.h

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

#ifndef _EOS_MEM_XXX
#define _EOS_MEM_XXX

#include "defines.h"
#include "print.h"

struct __memblknode
{
    uint64_t addr;  
    uint64_t len;   
    uint64_t size;  
    uint64_t type;  
};

struct __memblk
{
    struct __memblknode blocks[xx]; 
    uint32_t counts;               
};

typedef struct __memblk memconfig;

void * memcpy(void *From,void * To,long Num);

int memcmp(void * FirstPart,void * SecondPart,long Count);

void * memset(void * str,unsigned char C,long n);

void init_memory(memconfig *conf);
#endif