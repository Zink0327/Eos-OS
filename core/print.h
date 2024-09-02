/*----------EOS print function for kernel----------
    File name:print.h

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

#ifndef _PRINTF_K
#define _PRINTF_K

#include <stdarg.h>
#include "lib_k.h"
#include "font.h"
#include "global.h"

#define is_digit(c) ((c) >= '0' && (c) <= '9')
#define print_init(dst,src) \
    dst.fbaddr = src.screen.fbaddr;\
    dst.fblen = src.screen.fblen;\
    dst.x_res = src.screen.x_res;\
    dst.y_res = src.screen.y_res;\
    dst.bpp = src.screen.bpp;\
    dst.x_size = src.screen.x_size;\
    dst.y_size = src.screen.y_size;\
    dst.putchar_k = src.screen.putchar_k;


/* some token used by itoa_k() */
#define ZEROPAD	1		/* fill the rest with zero            00000001*/
#define SIGNED	2		/* unsigned/signed long               00000010*/
#define PLUS	4		/* show plus                          00000100*/
#define SPACE	8		/* replace plus with space            00001000*/
#define LEFT	16		/* left justified                     00010000*/
#define SPECIAL	32		/* base 8(0256) or base 16(0x256)     00100000*/
#define LOWERCASE	64  /* use 'abcdef' instead of 'ABCDEF'   01000000*/

/* some useful colors */
#define WHITE 	0xFFFFFF  // system notice
#define BLACK 	0x000000  // background color
#define RED	0xFF0000 	   // exception/function call failure
#define ORANGE	0xFF8000  // system fatal error
#define YELLOW	0xFFFF00  // exception explanation
#define GREEN	0x00FF00  // success
#define BLUE	0x0000FF
#define INDIGO	0x00FFFF  // initial function/subprocess notice
#define PURPLE	0x8000FF

/*
Prefix - How to use

prefix       usage
[ xxx] exception/interrupt 
<xxx>  function
--xx-- core step/core collapsed
*/

typedef void (*putchar_fun)(uint8_t*, uint8_t, uint16_t, int, int, uint32_t, uint32_t, uint8_t);
/*

Definition of each arguments

void putchar_k(
    uint8_t * fb,    // frame buffer (ignore if absent. For example, screen using i2c, spi, serial as their interface)
    uint8_t bpp,     // BIT per pixel (ignore if absent.) 
    uint16_t width,  // X resolution (must present)
    int x,           // x-coordinate (must present)
    int y,           // y-coordinate (must present)
    uint32_t FRcolor,// character's color (ignore if absent) 
    uint32_t BKcolor,// background color (ignore if absent) 
    uint8_t c        // character to be displayed (must present)

);

*/
struct _position {
	int16_t x_pos;
	int16_t y_pos;
};

struct _position position;

struct screenbkup {
    uint8_t *fbaddr;
    uint32_t fblen;
    uint16_t x_res;
    uint16_t y_res;
    uint8_t bpp;
    uint16_t x_size;
    uint16_t y_size;
    putchar_fun putchar_k;
} backupscrn;

/* buffer */
char tmpbuf[4096]={0};

/* transfer string to number */
int atoi_k(const char **s);

/* transfer number to string */
char * itoa_k(
    char * str,         /* address to place the transfered number */
    long num,           /* number to be transfered */
    int size,           /* length of the number in string */
    int base,           /* base */
    int precision,      /* the precision to decide how many digits after decimal point */
    int type            /* type above */
);


int vsprintf_k(char * buf,const char *fmt, va_list args);

/* print a string to a certain place in a format way */
int print(uint32_t fcolor, uint32_t bcolor,const char * _Format,...);


#endif
