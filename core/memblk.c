/*----------EOS memory processing module for BIOS----------
    File name:memblk.c

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

#include "memblk.h"

void init_memfragment(memfragment *frg, memblk *parent, uint32_t offset, uint64_t len, uint8_t attributes)
{
	frg->attributes = attributes;
	frg->offset = offset;
	frg->len = len;
	frg->endoffset = offset + len - 1;
	frg->parent = parent;
}

void new_memblk(memblk *block, List_memfragment *first_memfrg, enum memblk_type type)
{
    block -> fragments = first_memfrg;
    block -> fragcount = list_length_memfragment(first_memfrg);
}

uint64_t is_address_in_mmap(mmap *map, addrtype address)
{
	uint64_t res = mem_addr_not_found;
    for (uint64_t i = 0; i < map->bitmap_length; i++)
	{
		if (mem_addr_in_interval(address, map->blocks[i].address, map->blocks[i].address + map->blocksize))
		{
			res = i;
			break;
		}
	}
	return res;
}

void init_gmd()
{
    gmd.memory.blocksize=0;
    gmd.memory.block_bitmap = NULL;
    gmd.memory.bitmap_length = 0;
    gmd.memory.bitmap_len_byte = 0;
}

// @warning 
// DO NOT USE IT OUT OF THE CORE. Improper use of this function may result in memory collapse.
// @brief 
// Allocate small amount of memory for CORE variables
void *simple_malloc(addrtype number_of_bytes)
{
    // under construction
    void *tmp = (void *)gmd.heap;
	uint64_t present_place = is_address_in_mmap(&gmd.memory, (addrtype)tmp);
    gmd.heap += number_of_bytes;
    memset(tmp,0x00,number_of_bytes);
	uint64_t present_place2 = is_address_in_mmap(&gmd.memory, (addrtype)tmp) - present_place;
	if (present_place2 > 0)
	{
		mem_bitmap_set_range(present_place, present_place2)
	}
    return tmp;
}

static inline uint64_t skip_ones(uint64_t word)
{
	return ctzll(~word);
}


uint64_t find_zero_sequence(const uint64_t* bitmap, uint64_t bitmap_len, uint64_t required_len)
{
	// quick interval check
	if (required_len == 0 || bitmap_len == 0 || bitmap_len * WORD_BITS < required_len)
	{
		return -1;
	}
	uint64_t current_zeros = 0; // length of the window
	uint64_t window_start = 0;  // left slide of present window 
	uint64_t pos = 0; // right slide of present window 
	uint64_t res = mem_addr_not_found;
	uint64_t word_idx = 0; // index of word in `bitmap`
	uint64_t bit_offset = 0; // offset of `pos` in `word[word_idx]`
	uint64_t zero_offset = 0; // offset of the first 1 related to `pos`
	uint64_t word = 0; // slide

	while (pos < bitmap_len * WORD_BITS)
	{
		word_idx = pos >> 6;  // pos / WORD_BITS
		bit_offset = pos & WORD_MASK; // pos % WORD_BITS
		word = (bitmap[word_idx] >> bit_offset | bitmap[word_idx + 1] << (WORD_BITS - bit_offset));

		// if `word` equals 0
		if (word == 0)
		{
			uint64_t available = WORD_BITS - bit_offset;
			if ((current_zeros += available) >= required_len)
			{
				res = window_start;
				break;
			}
			pos += available;
			continue;
		}

		// if `word` equals 0xFFFFFFFFFFFFFFFF
		if (word == ~(uint64_t)(0))
		{
			current_zeros = 0;
			pos = window_start = (word_idx + 1) << 6;
			continue;
		}

		// mixed

		zero_offset = ctzll(word);
		if ((current_zeros += zero_offset) >= required_len)
		{
			res = window_start;
			break;
		}

		pos += zero_offset;
		pos += skip_ones(word >> zero_offset);
		current_zeros = 0;
		window_start = pos;
	}
	return res;
}

memfragment *alloc_memblk(uint64_t blockcounts, uint8_t attributes)
{
    uint64_t start = find_zero_sequence(gmd.memory.block_bitmap, gmd.memory.bitmap_length, blockcounts);
    if (start != mem_addr_not_found)
	{
		memfragment *res = (memfragment *)simple_malloc(sizeof(memfragment));
		memset(res, 0, sizeof(res));
		init_memfragment(res, &gmd.memory.blocks[start], 0, gmd.memory.blocksize, attributes);
        List_memfragment *l_res = (List_memfragment *)simple_malloc(sizeof(List_memfragment));
        memset(l_res, 0, sizeof(l_res));
        list_init_memfragment(l_res);
        list_data_copy_memfragment(l_res, res);
        for (uint32_t i = start; i < blockcounts; i++)
        {
            new_memblk(&gmd.memory.blocks[i], l_res, MEMBLK_TYPE_INCLUDED_BY_MEMFRG);
        }
        mem_bitmap_set_range(start, blockcounts)
		
		return res;
	}
	
    return NULL;
}