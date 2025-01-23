/*----------EOS memory processing module for BIOS----------
    File name:memblk.h

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

#ifndef _EOS_MEM_BLOCK_MAMAGEMENT
#define _EOS_MEM_BLOCK_MAMAGEMENT

#include "global.h"
#define mem_1k_shift 10
#define mem_1k_size (1UL << mem_1k_shift)
#define mem_1k_mask (~(mem_1k_size - 1))
#define mem_1k_align(addr)	(((uint64_t)(addr) + mem_1k_size - 1) & mem_1k_mask)

#define mem_8byte_align(addr) ((uint64_t)(addr + 63) / 8) & ( ~ 7 ); /* short of (((uint64_t)addr + sizeof(uint64_t) * 8 - 1) / 8) & ( ~ (sizeof(uint64_t) - 1)) */

#define mem_bitmap_set(num) *(gmd.block_bitmap + ((num) >> 6)) ^= 1UL << ((num) % 64);
/* defined in linkkernel.lds */
extern char _text;
extern char _etext;
extern char _edata;
extern char _end;

/*
 ______________________________________________
|                Memory (RAM only)             |
|_2k_|__2k_|__2k__|__2k__|__2k__|___2k__|__2k__|
 _________/        \__________________________
/_____________________________________________\
|           Memory Block (memblk)             |   The memory is divided into several blocks of same length, they're called memory block
|_free_|____used____|_______free_______|us|_f_|
 _____/             \_________________________
/_____________________________________________\
|         Memory Fragment (memfragment)       |   The memory block contains some continuous used memory, they're called memory fragment
|_____________________________________________|


*/

typedef struct _memfragment
{
    uint8_t presents;          /* 1:used 0:free */
    struct _memblk *parent;    /* parent memory block */
    uint32_t offset;           /* offset based on parent memblk's physical address */
    uint32_t endoffset;        /* the end of the fragment (use more space for less time, a simple trick to avoid #TLE ;-) )*/
    uint32_t len;              /* length of the memory fragment */
} memfragment;


typedef struct _memblk
{
    memfragment *fragments;
    uint32_t fragcount;

    addrtype address;
    uint32_t type;
} memblk;
 
struct _gmd
{
    memblk *blocks;
    uint64_t blkstructsize;
    uint64_t blocksize;
    addrtype *block_bitmap;
    uint64_t bitmap_length; /* unit: bit. The same as block_counts */
    uint64_t bitmap_len_byte; /* unit: uint64_t */

    addrtype *heap; /* top of the core heap */

    addrtype code_start;
    addrtype code_end;
    addrtype data_end;
    addrtype eoc;  //end of core
} gmd;

void init_gmd();

/*
alloc_memblk: allocate available memory blocks
argument: blockcounts - how many blocks to allocate
return value: the first block
*/

memblk *alloc_memblk(uint64_t blockcounts);
#endif