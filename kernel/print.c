/*----------Eos print function for kernel----------
    File name:print.c

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

#include "print.h"
#include "lib_k.h"
extern inline int strlen(char * String);

void putchar_k(unsigned int * fb,int Xsize,int x,int y,unsigned int FRcolor,unsigned int BKcolor,unsigned char fontn)
{
    int i = 0, j = 0, testval = 0;
    unsigned int *addr = NULL;
    unsigned char *fontp = NULL;
    fontp = font[fontn];

    for (i = 0; i < 16; i++)
    {
        addr = fb + Xsize * ( y + i ) + x;
        testval = 0x100;
        for (j = 0; j < 8; j++)
        {
            testval = testval >> 1;
            if (*fontp & testval)
            {
                *addr = FRcolor;
            }
            else
            {
                *addr = BKcolor;
            }
            addr++;
        }
        fontp++;
    }

}

int atoi_k(const char **s)
{
    int i=0;

    while (is_digit(**s))
        i = i*10 + *((*s)++) - '0';
    return i;
}

static char * itoa_k(char * str, long num, int base, int size, int precision,int type)
{

    char c,sign,tmp[50];

    /* counter */
    int i;
	
	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    if (type&LOWERCASE)
    {
        digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    }
    
    if (type&LEFT) type &= ~ZEROPAD;

    /* base can't be lower than 2 or higher than 36. If so, return. */
    if (base < 2 || base > 36)
        return -1;

    c = (type & ZEROPAD) ? '0' : ' ' ;

    /* do we need to add sign (+ or -) into the string? */
    sign = 0;
    if ((type & SIGNED) && (num < 0))
    {
        sign = '-';
        num = -num;
    }
    else
    {
        sign = (type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);
    }

    /* the sign will take up one bit of width */
    if (sign)
    {
        size--;
    }

    /* check if the base is 8 or 16 and do pre-processing */
    if (type & SPECIAL)
    {
        if (base == 16)
        {
            size -= 2;
        }
        else if (base == 8)
        {
            size--;
        }
    }

    /* write number into the tmp, backward */
    i = 0;
    if (num == 0)
    {
        tmp[ i++ ] = '0';
    }
    else
    {
        while (num != 0)
        {
            tmp[ i++ ] = digits[do_div(num,base)];
        }
    }

    if (i > precision)
    {
        precision = i;
    }
    size -= precision;

    /* write number data into the string */
    if (!(type & (ZEROPAD + LEFT))) /* neither "fill with zero" nor "left justified" */
    {
        while(size-- > 0)
        {
            *str++ = ' ';
        }
    }

    /* if the number is signed */
    if (sign)
    {
        *str++ = sign;
    }

    /* if the base is 8 or 16, write 0 or 0x */
    if (type & SPECIAL)
    {
        if (base == 8)
        {
            *str++ = '0'; /* write 0 when it's base 8 */
        }
        else if (base==16)
        {
            *str++ = '0';
            *str++ = digits[33]; /* write 0x when it's base 16 */
        }
    }

    if (!(type & LEFT))
    {
        while(size-- > 0)
        {
            *str++ = c;
        }
    }
    /* fill in with 0 to the specified width */
    while(i < precision--)
    {
        *str++ = '0';
    }
    /* write number data into the string, forward */
    while(i-- > 0)
    {
        *str++ = tmp[i];
    }
    /* fill in with space to the specified width */
    while(size-- > 0)
    {
        *str++ = ' ';
    }

    return str;
}



int vsprintf(char * buf,const char *fmt, va_list args)
{
    char * str,*s;
    int flags;
    int field_width;
    int precision;
    int len,i;

    int qualifier;		/* 'h', 'l', 'L' or 'Z' for integer fields */

    for(str = buf; *fmt; fmt++)
    {

        if(*fmt != '%')
        {
            *str++ = *fmt;
            continue;
        }
        flags = 0; /* *format = '%' */
repeat:
        fmt++; /* skip first '%' */
        switch(*fmt)
        {
        case '-':
        {
            flags |= LEFT;
            goto repeat;
        }
        case '+':
        {
            flags |= PLUS;
            goto repeat;
        }
        case ' ':
        {
            flags |= SPACE;
            goto repeat;
        }
        case '#':
        {
            flags |= SPECIAL;
            goto repeat;
        }
        case '0':
        {
            flags |= ZEROPAD;
            goto repeat;
        }
        }

        /* get field width */

        field_width = -1;
        if(is_digit(*fmt))
        {
            field_width = atoi_k(&fmt);
        }
        else if(*fmt == '*')
        {
            fmt++;
            field_width = va_arg(args, int);
            if(field_width < 0)
            {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision. the precision here is mainly used by string, not decimal fraction, because there isn't any */

        precision = -1;
        if(*fmt == '.')
        {
            fmt++;
            if(is_digit(*fmt))
            {
                precision = atoi_k(&fmt);
            }
            else if(*fmt == '*') /* set the precision due to the next argument... */
            {
                fmt++;
                precision = va_arg(args, int);
            }
            if(precision < 0)
                precision = 0;
        }


        qualifier = -1;
        if(*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'Z')  /* e.g. %ld lone integer %lf double float */
        {
            qualifier = *fmt;
            fmt++;
        }

        /* formatting the string... */

        switch(*fmt)
        {
        case 'c':

            if(!(flags & LEFT))
            {
                while(--field_width > 0)
                {
                    *str++ = ' ';
                }
            }
            *str++ = (unsigned char)va_arg(args, int);
            while(--field_width > 0)
            {
                *str++ = ' ';
            }
            break;

        case 's':

            s = va_arg(args,char *);
            if(!s)
            {
                s = '\0';
            }
            len = strlen(s);
            if(precision < 0)
            {
                precision = len;
            }
            else if(len > precision)
                len = precision;

            if(!(flags & LEFT))
            {
                while(len < field_width--)
                {
                    *str++ = ' ';
                }
            }
            for(i = 0; i < len ; i++)
                *str++ = *s++;
            while(len < field_width--)
                *str++ = ' ';
            break;

        case 'o':

            if(qualifier == 'l')
                str = itoa_k(str,va_arg(args,unsigned long),8,field_width,precision,flags);
            else
                str = itoa_k(str,va_arg(args,unsigned int),8,field_width,precision,flags);
            break;

        case 'p':

            if(field_width == -1)
            {
                field_width = 2 * sizeof(void *);
                flags |= ZEROPAD;
            }

            str = itoa_k(str,(unsigned long)va_arg(args,void *),16,field_width,precision,flags);
            break;

        case 'x':

            flags |= LOWERCASE;

        case 'X':

            if(qualifier == 'l')
                str = itoa_k(str,va_arg(args,unsigned long),16,field_width,precision,flags);
            else
                str = itoa_k(str,va_arg(args,unsigned int),16,field_width,precision,flags);
            break;

        case 'd':
        case 'i':

            flags |= SIGNED;
        case 'u':

            if(qualifier == 'l')
                str = itoa_k(str,va_arg(args,unsigned long),10,field_width,precision,flags);
            else
                str = itoa_k(str,va_arg(args,unsigned int),10,field_width,precision,flags);
            break;

        case 'n':

            if(qualifier == 'l')
            {
                long *ip = va_arg(args,long *);
                *ip = (str - buf);
            }
            else
            {
                int *ip = va_arg(args,int *);
                *ip = (str - buf);
            }
            break;

        case '%':

            *str++ = '%';
            break;

        default:

            *str++ = '%';
            if(*fmt)
                *str++ = *fmt;
            else
                fmt--;
            break;
        }

    }
    *str = '\0';
    return str - buf;
}


int print(unsigned int fcolor,unsigned int bcolor,const char * fmt,...)
{
	int i = 0;
	int count = 0;
	int line = 0;
	va_list args;
	va_start(args, fmt);

	i = vsprintf(buf,fmt, args);

	va_end(args);

	for(count = 0;count < i || line;count++)
	{
		////	add \n \b \t
		if(line > 0)
		{
			count--;
			goto _tab;
		}
		if((unsigned char)*(buf + count) == '\n')
		{
			position.y_pos++;
			position.x_pos = 0;
		}
		else if((unsigned char)*(buf + count) == '\b')
		{
			position.x_pos--;
			if(position.x_pos < 0)
			{
				position.x_pos = (position.x_res / position.x_size - 1) * position.x_size;
				position.y_pos--;
				if(position.y_pos < 0)
					position.y_pos = (position.y_res / position.y_size - 1) * position.y_size;
			}	
			putchar_k(position.fbaddr , position.x_res , position.x_pos * position.x_size , position.y_pos * position.y_size , fcolor , bcolor , ' ');	
		}
		else if((unsigned char)*(buf + count) == '\t')
		{
			line = ((position.x_pos + 8) & ~(8 - 1)) - position.x_pos;

_tab:
			line--;
			putchar_k(position.fbaddr , position.x_res , position.x_pos * position.x_size , position.y_pos * position.y_size , fcolor , bcolor , ' ');	
			position.x_pos++;
		}
		else
		{
			putchar_k(position.fbaddr , position.x_res , position.x_pos * position.x_size , position.y_pos * position.y_size , fcolor , bcolor , (unsigned char)*(buf + count));
			position.x_pos++;
		}


		if(position.x_pos >= (position.x_res / position.x_size))
		{
			position.y_pos++;
			position.x_pos = 0;
		}
		if(position.y_pos >= (position.y_res / position.y_size))
		{
			position.y_pos = 0;
		}

	}
	return i;
}

