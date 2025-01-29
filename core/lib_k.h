/*----------EOS library for kernel----------
    File name:lib_k.h

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
#ifndef __LIB_K_H__
#define __LIB_K_H__

#include "global.h"

#define container_of(ptr,type,member)							\
({											\
	typeof(((type *)0)->member) * p = (ptr);					\
	(type *)((unsigned long)p - (unsigned long)&(((type *)0)->member));		\
})

#ifdef x86_64
void io_sti();
void io_cli();
void io_nop();
void io_mfence();
#endif

/* something about list processing... */
#include "global.h"

struct List
{
	struct List * prev;
	struct List * next;
};

inline void list_init(struct List * list)
{
	list->prev = list;
	list->next = list;
}

inline void list_append(struct List * entry,struct List * new)	////add to entry behind
{
	new->next = entry->next;
	new->prev = entry;
	new->next->prev = new;
	entry->next = new;
}

inline void list_insert_to_head(struct List * entry,struct List * new)
{
	new->next = entry;
	entry->prev->next = new;
	new->prev = entry->prev;
	entry->prev = new;
}

inline void list_del(struct List * entry)
{
	entry->next->prev = entry->prev;
	entry->prev->next = entry->next;
}

inline long is_list_empty(struct List * entry)
{
	if(entry == entry->next && entry->prev == entry)
		return 1;
	else
		return 0;
}

inline struct List * list_prev(struct List * entry)
{
	if(entry->prev != NULL)
		return entry->prev;
	else
		return NULL;
}

inline struct List * list_next(struct List * entry)
{
	if(entry->next != NULL)
		return entry->next;
	else
		return NULL;
}

/* over */
/* something about string processing... */


/*
copy src to dest
*/

inline char * strcpy(char * Dest,char * Src)
{
#ifdef x86_64
	__asm__	__volatile__	(	"cld			\n\t"
					"1:			\n\t"
					"lodsb			\n\t"
					"stosb			\n\t"
					"testb	%%al, %%al	\n\t"
					"jne	1b		\n\t"
					::"S"(Src),"D"(Dest):
				);
	return 	Dest;
#else
	for (; *Src != '\0'; ++Dest, ++Src)
		*Dest = *Src;
	*Dest = '\0';
	return Dest;
#endif
}

/*
Copy the first n bytes from src to dest
*/

inline char * strncpy(char * Dest,char * Src,long n)
{
#ifdef x86_64
	__asm__	__volatile__	(	"cld			\n\t"
					"1:			\n\t"
					"decq	%[count]	\n\t"
					"js	2f		\n\t"
					"lodsb			\n\t"
					"stosb			\n\t"
					"testb	%%al, %%al	\n\t"
					"jne	1b		\n\t"
					"rep			\n\t"
					"stosb			\n\t"
					"2:			\n\t"
					::"S"(Src),"D"(Dest),[count] "c"(n):					
				);
	return Dest;
#else
	for (; *Src != '\0' && n > 0; ++Dest, ++Src, --n)
		*Dest = *Src;
	*Dest = '\0';
	return Dest;
#endif 
}

/*
Append src to dest
*/

inline char * strcat(char * Dest,char * Src)
{
#ifdef x86_64
	__asm__	__volatile__	(	"cld			\n\t"
					"repne			\n\t"
					"scasb			\n\t"
					"decq	%[rdi]		\n\t"
					"1:			\n\t"
					"lodsb			\n\t"
					"stosb			\n\r"
					"testb	%%al, %%al	\n\t"
					"jne	1b		\n\t"
					:
					:[rsi] "S"(Src),[rdi] "D"(Dest),"a"(0),"c"(0xffffffff)
					:					
				);
	return Dest;
#else
	while (*Dest != 0)
		++Dest;

	for (; *Src != '\0'; ++Dest, ++Src)
		*Dest = *Src;
	*Dest = '\0';
	return Dest;
#endif
}

/*
		compare FirstPart and SecondPart
		FirstPart = SecondPart =>  0
		FirstPart > SecondPart =>  1
		FirstPart < SecondPart => -1
*/

int strcmp(char * FirstPart,char * SecondPart)
{
#ifdef x86_64
	register int __res;
	__asm__	__volatile__	(	"cld			\n\t"
					"1:			\n\t"
					"lodsb			\n\t"
					"scasb			\n\t"
					"jne	2f		\n\t"
					"testb	%%al,	%%al	\n\t"
					"jne	1b		\n\t"
					"xorl	%%eax,	%%eax	\n\t"
					"jmp	3f		\n\t"
					"2:			\n\t"
					"movl	$1,	%%eax	\n\t"
					"jl	3f		\n\t"
					"negl	%%eax		\n\t"
					"3:			\n\t"
					:"=a"(__res):"D"(FirstPart),"S"(SecondPart):
				);
	return __res;
#else
	unsigned char *s0 = (unsigned char *)FirstPart;
	unsigned char *s1 = (unsigned char *)SecondPart;
	int d = 0;

	for(;(*s0 || *s1)&&(d != 0);++s0,++s1)
		d = *s0 - *s1;

	return d;
#endif
}

/*
		compare n bytes of FirstPart and SecondPart
		FirstPart = SecondPart =>  0
		FirstPart > SecondPart =>  1
		FirstPart < SecondPart => -1
*/

int strncmp(char * FirstPart,char * SecondPart,long n)
{	
#ifdef x86_64
	register int __res;
	__asm__	__volatile__	(	"cld			\n\t"
					"1:			\n\t"
					"decq	%3		\n\t"
					"js	2f		\n\t"
					"lodsb			\n\t"
					"scasb			\n\t"
					"jne	3f		\n\t"
					"testb	%%al,	%%al	\n\t"
					"jne	1b		\n\t"
					"2:			\n\t"
					"xorl	%%eax,	%%eax	\n\t"
					"jmp	4f		\n\t"
					"3:			\n\t"
					"movl	$1,	%%eax	\n\t"
					"jl	4f		\n\t"
					"negl	%%eax		\n\t"
					"4:			\n\t"
					:"=a"(__res):"D"(FirstPart),"S"(SecondPart),"c"(n):
				);
	return __res;
#else
	unsigned char c0,c1;
	char r = 0
	for(;n;--n,++FirstPart,++SecondPart)
	{
		c0 = *FirstPart;
		c1 = *SecondPart;
		if(c0 != c1)
		{
			r = c0 < c1 ? -1 : 1;
			break;
		}
		if(!c0)
			break;

	}
	return r;
#endif
}

int strlen(char * String)
{
#ifdef x86_64
	register int __res;
	__asm__	__volatile__	(	"cld		\n\t"
					"repne		\n\t"
					"scasb		\n\t"
					"notl	%[len]	\n\t"
					"decl	%[len]	\n\t"
					:[len] "=c"(__res)
					:"D"(String),"a"(0),"0"(0xffffffff)
					:
				);
	return __res;
#else
	char *e = String;
	for(;*e != '\0';++e)
		;
	return (e - String);
#endif
}

/* over */

inline unsigned long bit_set(unsigned long * addr,unsigned long nr)
{
	return *addr | (1UL << nr);
}

inline unsigned long bit_get(unsigned long * addr,unsigned long nr)
{
	return	*addr & (1UL << nr);
}

inline unsigned long bit_clean(unsigned long * addr,unsigned long nr)
{
	return	*addr & (~(1UL << nr));
}

uint8_t io_in8(uint16_t port);
uint16_t io_in16(uint16_t port);
uint32_t io_in32(uint16_t port);
void io_out8(uint16_t port,uint8_t value);
void io_out16(uint16_t port,uint16_t value);
void io_out32(uint16_t port,uint32_t value);

#ifdef x86_64

#define port_insw(port,buffer,nr)	\
__asm__ __volatile__("cld;rep;insw;mfence;"::"d"(port),"D"(buffer),"c"(nr):"memory")

#define port_outsw(port,buffer,nr)	\
__asm__ __volatile__("cld;rep;outsw;mfence;"::"d"(port),"S"(buffer),"c"(nr):"memory")

#endif



#endif
