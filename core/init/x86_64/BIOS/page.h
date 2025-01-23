/*----------EOS page definition for BIOS----------
    File name:page.h

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
#ifndef __EOS_PAG_DEF
#define __EOS_PAG_DEF

#include "defines.h"

#define POINTERS_PER_PAGE 512

#define PAGE_2M_SHL(x) ((x) << 21)
#define PAGE_2M_SHR(x) ((x) >> 21)

#define PAGE_2M_SIZE PAGE_2M_SHL(1UL) // 0x200000
#define PAGE_2M_MASK (~(PAGE_2M_SIZE - 1)) //erase the low 21 bit
#define PAGE_2M_ALIGN(a) (((uint64_t)(a) + PAGE_2M_SIZE - 1) & PAGE_2M_MASK)

#endif
