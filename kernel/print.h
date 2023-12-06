/*----------Eos print function for kernel----------
    File name:print.h

    Copyright (C) 2023 by Zink
    This file is part of Eos

    Eos is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Eos is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef _PRINTF_K
#define _PRINTF_K

#include <stdarg.h>
#include "linkage.h"
#include "lib_k.h"
#include "font.h"

#define is_digit(c) ((c) >= '0' && (c) <= '9')

/* some token */
#define ZEROPAD	1		/* fill with zero */
#define SIGNED	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* base 8(0256) or base 16(0x256) */
#define LOWERCASE	64		/* use 'abcdef' instead of 'ABCDEF' */

/* some useful colors */
#define WHITE 	0xFFFFFF
#define BLACK 	0x000000
#define RED	0xFF0000		
#define ORANGE	0xFF8000		
#define YELLOW	0xFFFF00		
#define GREEN	0x00FF00		
#define BLUE	0x0000FF
#define INDIGO	0x00FFFF
#define PURPLE	0x8000FF

/* font data */
/* extern unsigned char font[256][16]; */

/* buffer */
char buf[4096]={0};

struct _position{
	/* resolution */
	int x_res;
	int y_res;
	/* position */
	int x_pos;
	int y_pos;
	/* size */
	int x_size;
	int y_size;

	unsigned int *fbaddr;
	unsigned long fblen;

};

struct _position position;

void putchar_k(unsigned int * fb,int Xsize,int x,int y,unsigned int FRcolor,unsigned int BKcolor,unsigned char fontn);

/* transfer string to number */
int atoi_k(const char **s);

/* do divition */
#define do_div(n,base) ({ \
int __res; \
__asm__("divq %%rcx":"=a" (n),"=d" (__res):"0" (n),"1" (0),"c" (base)); \
__res; })

/* transfer number to string */
static char * itoa_k(char * str, long num, int base, int size, int precision ,int type);


int vsprintf_k(char * buf,const char *fmt, va_list args);

/* print a string in a format way */
int print(unsigned int fcolor,unsigned int bcolor,const char * _Format,...);


#endif
