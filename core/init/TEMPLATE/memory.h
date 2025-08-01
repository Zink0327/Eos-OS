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
    uint32_t ramcounts;
};

typedef struct __memblk memconfig;

/* memblk gmd struct initialize. Differ fron the memconfig's struct */
#define memblk_init(src) \
    gmd.memory.bitmap_length = src.ramcounts; \
    for (uint32_t i = 0,j = 0,k = 0; j < src.counts; j++) \
    { \
        if (src.blocks[j].type == 1) \
        { \
            gmd.memory.blocksize = src.blocks[j].size; \
            uint32_t l = src.blocks[j].len; \
            while(k < l) \
            { \
                gmd.memory.blocks[i].address = src.blocks[j].addr + src.blocks[j].size * k; \
                gmd.memory.blocks[i].type = 1; \
                gmd.memory.blocks[i].fragments = NULL; \
                gmd.memory.blocks[i++].fragcount = 0; \
            } \
        } \
        k = 0; \
    }
    
/* realized in asmfun.asm/asmfun.S */

void * memcpy(void *From,void * To,long Num);

int memcmp(void * FirstPart,void * SecondPart,long Count);

void * memset(void * str,unsigned char C,long n);

void init_memory(memconfig *conf);
#endif