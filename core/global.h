/*----------EOS global definition----------
    File name:global.h

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

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "linkage.h"

typedef long DWORD;
typedef int WORD;
typedef char BYTE;






/* Feel free to edit the following configs */

#define EOS_CHIP_STRUCTURE x86_64
#define EOS_BOOT_WAY BIOS


/* end of configs */


#include SYMBOL_NAME_STR2(init/EOS_CHIP_STRUCTURE/EOS_BOOT_WAY/init.h)

#endif
