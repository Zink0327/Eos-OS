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
#include "memory.h"
#include "lib_k.h"
#include "print.h"

#include "list.h"
#define WORD_BITS (sizeof(uint64_t) * CHAR_BIT)
#define WORD_MASK (WORD_BITS - 1)

#define mem_1k_shift 10
#define mem_1k_size (1UL << mem_1k_shift)
#define mem_1k_mask (~(mem_1k_size - 1))
#define mem_1k_align(addr)	(((addrtype)(addr) + mem_1k_size - 1) & mem_1k_mask)

#define mem_align(addr,size) (((addr) + size - 1) & (~(size - 1)))

#define mem_bit_size(n) (((addrtype)((n) + sizeof(addrtype)*8 - 1)/sizeof(addrtype)) & ( ~ (sizeof(addrtype) - 1) ))

#define mem_addr_in_interval(addr, lower, upper) ((int64_t) ((((uint64_t)addr - (uint64_t)(lower))) | ((uint64_t)(upper) - (uint64_t)addr))) >= 0 // addr in [lower, upper]

/*
notice:when we use mem_bitmap_set and mem_bitmap_clear, we consider the order of bit like this:
bit no.  15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
        |0  0  0  0  0  0  0 0|0 0 0 0 0 0 0 0|
to set the bit no.5, for example
mem_bitmap_set(5)
and the memory will look like this:
bit no.  15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
        |0  0  0  0  0  0  0 0|0 0 1 0 0 0 0 0|
*/
#define mem_bitmap_set(num) *(gmd.memory.block_bitmap + ((num) >> 6)) |= (1UL << ((num) % 64));
#define mem_bitmap_clear(num) *(gmd.memory.block_bitmap + ((num) >> 6)) &=( ~(1UL << ((num) % 64)));
#define mem_bitmap_set_range(num, range) *(gmd.memory.block_bitmap + ((num) >> 6)) |= (((1UL << range) - 1) << ((num) % 64));

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
|         Memory Fragment (memfragment)       |   The memory block contains/is contained by some continuous used memory, they're called memory fragment
|_____________________________________________|


*/

typedef struct _memfragment
{
          
     /* bit 0-1:  memory fragment type (enum fragment_type)
        bit 2  :
    */
    uint8_t attributes;   
    struct _memblk *parent;    /* parent memory block / first child memory block */
    uint32_t offset;           /* offset based on parent memblk's physical address */
    uint32_t endoffset;        /* the end of the fragment (use more space for less time, a simple trick to avoid #TLE ;-) )*/
    uint64_t len;              /* length of the memory fragment (KiB) / How many memory blocks it contains */
} memfragment;


enum fragment_type
{
    FRAGMENT_TYPE_FREE = 0,             /* Unused / Free memory fragment */
    FRAGMENT_TYPE_MEMBLK_SPANNED,       /* This memory fragment uses memory block as unit */
    FRAGMENT_TYPE_INCLUDED_BY_MEMBLK    /* This memory fragment is included by memory block */
};

enum fragment_level
{
    FRAGMENT_LEVEL_CORE = 0,
    FRAGMENT_LEVEL_USER
};

enum fragment_visibility
{
    FRAGMENT_VISIBILITY_PRIVATE = 0,
    FRAGMENT_VISIBILITY_PUBLIC
};

#define fragment_set_attribute(type,level,visibility,c) (type | level << 2 | visibility << 3 | c << 4) 

DECLARE_LIST(memfragment)
IMPLEMENT_LIST(memfragment)
void list_data_copy_memfragment(List_memfragment *entry, memfragment *data)
{
    entry->data.attributes = data->attributes;
    entry->data.endoffset = data->endoffset;
    entry->data.len = data->len;
    entry->data.offset = data->offset;
    entry->data.parent = data->parent;
}

typedef struct _memblk
{
    List_memfragment *fragments;   // parent fragment / first child fragment
    int32_t fragcount;       // how many fragments it contains

    addrtype *address;
    uint32_t type;
} memblk;

enum memblk_type
{
    MEMBLK_TYPE_FREE = 0,             /* Unused / Free memory fragment */
    MEMBLK_TYPE_ONE_TO_ONE,           /* This memory fragment is combined to one memory block */
    MEMBLK_TYPE_MEMFRG_SPANNED,       /* This memory fragment uses memory block as unit */
    MEMBLK_TYPE_INCLUDED_BY_MEMFRG    /* This memory fragment is included by memory block */
};

void init_memfragment(memfragment *frg, memblk *parent, uint32_t offset, uint64_t len, uint8_t attributes);

/*
 Assign a memblk with a series of memfragment
*/
void new_memblk(memblk *block, List_memfragment *first_memfrg, enum memblk_type type);
/*
 Free a memblk and return the head of memfragment it contains to free.
*/
List_memfragment * free_memblk(memblk *block);

typedef struct _mmap
{
    memblk *blocks;           /* an array recording the memory blocks */
    uint64_t blkstructsize;   /* how much memory does the `blocks` array takes */
    uint64_t blocksize;       /* size of each memory block recorded in `blocks`. Unit: KiB */
    addrtype *block_bitmap;   /* literally. Set if used, clear if free */
    uint64_t bitmap_length;   /* unit: bit. How much records does `blocks` contain */
    uint64_t bitmap_len_byte; /* unit: uint64_t */
} mmap;

#define mem_addr_not_found (uint64_t)0xFFFFFFFFFFFFFFFF
uint64_t is_address_in_mmap(mmap *map, addrtype address);

 
struct _gmd
{
    mmap memory;

    addrtype heap; /* top of the core heap */

    addrtype code_start;
    addrtype code_end;
    addrtype data_end;
    addrtype eoc;  //end of core
} gmd;

void init_gmd();

void *simple_malloc(addrtype number_of_bytes);
/*
alloc_memblk: allocate available memory blocks
arguments: 
blockcounts - how many blocks to allocate
return value: the first block
*/
memfragment *alloc_memblk(uint64_t blockcounts, uint8_t attibutes);
#endif