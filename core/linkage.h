/*----------EOS linkage for kernel----------
    File name:linkage.h

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

#ifndef _LINKAGE_H_
#define _LINKAGE_H_

#define L1_CACHE_BYTES 32

#define asmlinkage __attribute__((regparm(0)))	

#define ____cacheline_aligned __attribute__((__aligned__(L1_CACHE_BYTES)))

#define SYMBOL_NAME(X)	X     // do nothing

#define SYMBOL_NAME_STR(X)	#X //convert X to a string

#define SYMBOL_NAME_STR2(X)	SYMBOL_NAME_STR(X)	 //convert X to a string(macro)

#define SYMBOL_NAME_LABEL(X) X##:  //join X with ':'

#define SYMBOL_NAME_COMBINE(X,Y) X##Y  //join X with Y

#define SYMBOL_NAME_COMBINE2(X,Y)  SYMBOL_NAME_COMBINE(X,Y)     //join macros together

#define ENTRY(name)		\
.global	SYMBOL_NAME(name);	\
SYMBOL_NAME_LABEL(name)

#endif
