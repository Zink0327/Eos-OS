/*----------EOS print function for kernel----------
	File name:print.c

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

#include "print.h"
#include "lib_k.h"
extern inline int strlen(char* String);


int atoi_k(const char** s)
{
	int i = 0;

	while (is_digit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

char* itoa_k(char* str, long num, int size, int base, int precision, int type)
{

	char c = 0, sign = 0, tmp[256] = { 0 };

	/* counter */
	int i = 0, offset = size;
	unsigned int n = 0;

	const char* digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	if (type & LOWERCASE)
	{
		digits = "0123456789abcdefghijklmnopqrstuvwxyz";  /* that doesn't cause an error, what a surprise... */
	}

	if (type & LEFT) type &= ~ZEROPAD;

	/* base can't be lower than 2 or higher than 36. If so, return. */
	if (base < 2 || base > 36)
		return NULL;

	c = (type & ZEROPAD) ? '0' : ' ';

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
		offset--;
	}

	/* check if the base is 8 or 16 and do pre-processing */
	if (type & SPECIAL)
	{
		if (base == 16)
		{
			offset -= 2;
		}
		else if (base == 8)
		{
			offset--;
		}
	}

	/* write number into the tmp, backward */
	i = 0;
	if (num == 0)
	{
		tmp[i++] = '0';
	}
	else
	{
		unsigned long nnn = num;
		while (nnn != 0)
		{
			n = nnn % base;
			tmp[i++] = digits[n];
/*   to calculate 23 based 10 to base 2, for example
                         ^
       2 |__2_3__ ......1|
       2 |__1_1__ ......1|
        2 |__5__  ......1|
        2 |__2__  ......0|
             1           |
             -------------

	   23 based 10 = 10111 based 2

	  but in tmp[], the digits are as below:
			 { 1 , 1 , 1 , 0 , 1 }
	  As you see, the digits are backward, so we have to reverse the digits later.
*/
			nnn = (nnn - n) / base;
		}
	}

	int tmp_len = precision;

    if (i > tmp_len)
	{
		tmp_len = i;
	}
	if (offset < tmp_len && offset >= 0)
	{
		i += offset - tmp_len;
		offset -= tmp_len;
		tmp_len = i;
	}
	else
	{
		offset -= tmp_len;
	}


	/* write number data into the string */
	if (!(type & (ZEROPAD + LEFT))) /* neither "fill with zero" nor "left justified" */
	{
		while (offset-- > 0)
		{
			*str = ' ';
			++str;
		}
	}

	/* if the number is signed */
	if (sign)
	{
		*str = sign;
		++str;
	}

	/* if the base is 8 or 16, write 0 or 0x */
	if (type & SPECIAL)
	{
		if (base == 8)
		{
			*str = '0'; /* write 0 when it's base 8 */
			++str;
		}
		else if (base == 16)
		{
			*str = '0';
			++str;
			*str = digits[33]; /* write 0x when it's base 16 */
			++str;
		}
	}

	if (!(type & LEFT))
	{
		while (offset-- > 0)
		{
			*str = c;
			++str;
		}
	}
	/* fill in with 0 to the specified width */
	while (i < tmp_len--)
	{
		*str = '0';
		++str;
	}
	/* write number data into the string, forward */
	while (i-- > 0)
	{
		*str = tmp[i];
		++str;
	}
	/* fill in with space to the specified width */
	while (offset-- > 0)
	{
		*str = ' ';
		++str;
	}

	return str;
}



int vsprintf_k(char* buf, const char* fmt, va_list args)
{
	char* str = NULL, * s = NULL;
	int flags = 0, field_width = 0, precision = 0, len = 0, i = 0;
	int qualifier = 0;		/* 'h', 'l', 'L' or 'Z' for integer fields */

	if (fmt == NULL)
		return 0;

	for (str = buf; *fmt; fmt++)
	{

		if (*fmt != '%')
		{
			*str = *fmt;
			++str;
			continue;
		}
		flags = 0; /* *format = '%' */
		repeat:
		fmt++; /* skip first '%' */
		switch (*fmt)
		{
		case '-':
		
			flags |= LEFT;
			goto repeat;

		case '+':

			flags |= PLUS;
			goto repeat;

		case ' ':

			flags |= SPACE;
			goto repeat;

		case '#':

			flags |= SPECIAL;
			goto repeat;

		case '0':

			flags |= ZEROPAD;
			goto repeat;

		}

		/* get field width to decide how many digits to be displayed */

		field_width = -1;
		if (is_digit(*fmt))
		{
			field_width = atoi_k(&fmt);
		}
		else if (*fmt == '*')     /* if it's "%*(dxof...)", then replace it with the next integer argument */
		{
			fmt++;
			field_width = va_arg(args, int);
			if (field_width < 0)
			{
				field_width = -field_width;
				flags |= LEFT;
			}
			if (field_width > 256)
			{
				field_width = 256;
			}
		}

		/* get the precision to decide how many digits after decimal point. the precision here is mainly used by string, not decimal fraction, because there isn't any */

		precision = -1;
		if (*fmt == '.')
		{
			fmt++;
			if (is_digit(*fmt))
			{
				precision = atoi_k(&fmt);
			}
			else if (*fmt == '*') /* set the precision according to the next integer argument... */
			{
				fmt++;
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* decide how many bytes the argument take. */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'Z')  /* e.g. %ld long integer %lf double float */
		{
			qualifier = *fmt;
			fmt++;
		}

		/* formatting the string... */

		switch (*fmt)
		{
		case 'c':         /*%c*/

			if (!(flags & LEFT))
			{
				while (--field_width > 0)
				{
					*str = ' ';
					++str;
				}
			}
			*str  = (uint8_t)va_arg(args, int);
			++str;
			while (--field_width > 0)
			{
				*str = ' ';
				++str;
			}
			break;

		case 's':

			s = va_arg(args, char*);
			if (!s)
			{
				s = '\0';
			}
			len = strlen(s);
			if (precision < 0)       
			{
				precision = len;
			}
			else if (len > precision)
				len = precision;

			if (!(flags & LEFT))
			{
				while (len < field_width--)
				{
					*str = ' ';
					++str;
				}
			}
			for (i = 0; i < len; i++)
			{
				*str = *s;
				++str;
				++s;
			}
			while (len < field_width--)
			{
				*str = ' ';
				++str;
			}
			break;

		case 'b':

			if (qualifier == 'l')
				str = itoa_k(str, va_arg(args, uint64_t), field_width, 2, precision, flags);
			else
				str = itoa_k(str, va_arg(args, uint32_t), field_width, 2, precision, flags);
			break;

		case 'o':

			if (qualifier == 'l')
				str = itoa_k(str, va_arg(args, uint64_t), field_width, 8, precision, flags);
			else
				str = itoa_k(str, va_arg(args, uint32_t), field_width, 8, precision, flags);
			break;

		case 'p':

			if (field_width == -1)
			{
				field_width = 2 * sizeof(void*);
				flags |= ZEROPAD;
			}

			str = itoa_k(str, (uint64_t)va_arg(args, void*), field_width, 16, precision, flags);
			break;

		case 'x':

			flags |= LOWERCASE;

		case 'X':

			if (qualifier == 'l')
				str = itoa_k(str, (uint64_t)va_arg(args, uint64_t), field_width, 16, precision, flags);
			else
				str = itoa_k(str, (uint64_t)va_arg(args, uint32_t), field_width, 16, precision, flags);
			break;

		case 'd':
		case 'i':

			flags |= SIGNED;
		case 'u':

			if (qualifier == 'l')
				str = itoa_k(str, va_arg(args, int64_t), field_width, 10, precision, flags);
			else
				str = itoa_k(str, va_arg(args, int32_t), field_width, 10, precision, flags);
			break;

		case 'n':

			if (qualifier == 'l')
			{
				long* ip = va_arg(args, long*);
				*ip = (str - buf);
			}
			else
			{
				int* ip = va_arg(args, int*);
				*ip = (str - buf);
			}
			break;

		case '%':

			*str = '%';
			++str;
			break;

		default:

			*str = '%';
			++str;
			if (*fmt)
			{
				*str = *fmt;
				++str;
			}
			else
				fmt--;
			break;
		}
	}
	*str = '\0';
	return str - buf;
}


int print(uint32_t fcolor, uint32_t bcolor, const char* _Format, ...)
{
	int32_t i = 0, count = 0, line = 0;
	uint8_t ifnbt = 0;
	va_list args;
	if (backupscrn.y_size <= 0 || backupscrn.x_size <= 0)
	{
		return -1;
	}
	
	va_start(args, _Format);

	i = vsprintf_k(tmpbuf, _Format, args);

	va_end(args);

	for (count = 0; count < i || line; count++)
	{
		////	add \n \b \t
		ifnbt = (uint8_t)(*(tmpbuf + count));

		if (line > 0)
		{
			count--;
			ifnbt = (uint8_t)(*(tmpbuf + count));
			goto _tab;
		}
		if (ifnbt == '\n')
		{
			position.y_pos++;
			position.x_pos = 0;
		}
		else if (ifnbt == '\b')
		{
			position.x_pos--;
			if (position.x_pos < 0)
			{
				position.x_pos = (backupscrn.x_res / backupscrn.x_size - 1) * backupscrn.x_size;
				position.y_pos--;
				if (position.y_pos < 0)
					position.y_pos = (backupscrn.y_res / backupscrn.y_size - 1) * backupscrn.y_size;
			}
			backupscrn.putchar_k(backupscrn.fbaddr, backupscrn.bpp, backupscrn.x_res, position.x_pos * backupscrn.x_size, position.y_pos * backupscrn.y_size, fcolor, bcolor, ' ');
		}
		else if (ifnbt == '\t')
		{
			line = ((position.x_pos + 8) & ~(8 - 1)) - position.x_pos;

_tab:
			line--;
			backupscrn.putchar_k(backupscrn.fbaddr, backupscrn.bpp, backupscrn.x_res, position.x_pos * backupscrn.x_size, position.y_pos * backupscrn.y_size, fcolor, bcolor, ' ');
			position.x_pos++;
		}
		else
		{
			backupscrn.putchar_k(backupscrn.fbaddr, backupscrn.bpp, backupscrn.x_res, position.x_pos * backupscrn.x_size, position.y_pos * backupscrn.y_size, fcolor, bcolor, ifnbt);
			position.x_pos++;
		}


		if (position.x_pos >= (backupscrn.x_res / backupscrn.x_size))
		{
			position.y_pos++;
			position.x_pos = 0;
		}
		if (position.y_pos >= (backupscrn.y_res / backupscrn.y_size))
		{
			position.y_pos = 0;
		}

	}
	return i;
}

