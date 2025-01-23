/*----------EOS memory processing module for BIOS----------
    File name:memory.h

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
#ifndef _EOS_MEM_MODUIUBJHBHJBHBH
#define _EOS_MEM_MODUIUBJHBHJBHBH

#include "defines.h"
#include "print.h"
#include "page.h"

struct _memory_struct
{
    uint64_t BaseAddr;
	uint64_t Length;
	uint32_t Type;
} __attribute__ ((packed));

typedef struct _memory_struct memstruct;

struct __memblknode
{
    uint64_t addr;
    uint64_t len;
    uint64_t size;  //unit:1024 Byte
    uint64_t type;
};

struct __memblk
{
    struct __memblknode blocks[32];
    uint32_t counts;
    uint32_t ramcounts;
};

typedef struct __memblk memconfig;

/* memblk gmd struct initialize. Differ from the memconfig's struct */

#define memblk_get_length(src) gmd.bitmap_length = src.ramcounts;

#define memblk_init(src)                                                             \
    for (uint32_t i = 0,j = 0,k = 0; j < src.counts; j++)                            \
    {                                                                                \
        if (src.blocks[j].type == 1)                                                 \
        {                                                                            \
            gmd.blocksize = src.blocks[j].size;                                      \
            uint32_t l = src.blocks[j].len;                                          \
            for(k = 0;k < l;k++, i++)                                                \
            {                                                                        \
                gmd.blocks[i].address = src.blocks[j].addr + 1024 * src.blocks[j].size * k;    \
                gmd.blocks[i].type = 1;                                              \
                gmd.blocks[i].fragments = NULL;                                      \
                gmd.blocks[i].fragcount = 0;                                         \
                if(gmd.blocks[i].address < (addrtype)gmd.heap)                       \
                    mem_bitmap_set(i)                                                \
            }                                                                        \
        }                                                                            \
    }

/* realized in asmfun.asm/asmfun.S */

void * memcpy(void *From,void * To,long Num);

/*
		FirstPart = SecondPart		=>	 0
		FirstPart > SecondPart		=>	 1
		FirstPart < SecondPart		=>	-1
*/
int memcmp(void * FirstPart,void * SecondPart,long Count);

/*Copy C to memory at the first n bytes of str*/

void * memset(void * str,unsigned char C,unsigned long n);

void init_memory(memconfig *conf);
#endif