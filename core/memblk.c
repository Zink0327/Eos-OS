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

void init_gmd()
{
    gmd.blocksize=0;
    gmd.block_bitmap = NULL;
    gmd.bitmap_length = 0;
    gmd.bitmap_len_byte = 0;
}

memblk *alloc_memblk(uint64_t blockcounts)
{
    uint64_t *copy = gmd.block_bitmap;
}