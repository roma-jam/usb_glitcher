/*
 * lcd_format.c
 *
 *  Created on: 15 февр. 2018 г.
 *      Author: RLeonov
 */

#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>

static inline char *put_uint(char *p, unsigned int n, unsigned int base, int width, bool zero_padded)
{
    char digits[10];
    int len = 0;
    do
    {
        unsigned int digit = n%base;
        n /= base;
        digits[len++] = digit < 10 ? '0'+digit : 'A'+digit-10;
    } while (n > 0);

    for (int i = len; i < width; i++)
    {
        if (zero_padded)
            *p++ = '0';
        else
            *p++ = ' ';
    }

    while (len > 0)
        *p++ = digits[--len];
    return p;
}

int __vsprintf(char *buf, const char *format, va_list args)
{
    const char *f = format;
    char *p = buf;
    char c;
    while ((c = *f++) != 0)
    {
        if (c != '%')
        {
            *p++ = c;
            continue;
        }

        // Here goes optional width specification.
        // If it starts with zero (zero_padded is true), it means we use '0'
        // instead of ' ' as a filler.
        int width = 0;
        bool zero_padded = false;
        while (true)
        {
            c = *f++;
            if (c < '0' || c > '9')
                break;
            if (width == 0 && c == '0')
                zero_padded = true;
            width *= 10;
            width += c-'0';
        }

        if (c == 's')
        {
            char *s = va_arg(args, char*);
            while (*s != 0)
                *p++ = *s++;
        }
        else if (c == 'c')
        {
            *p++ = va_arg(args, int);
        }
        else if (c == 'X')
        {
            unsigned int n = va_arg(args, unsigned int);
            p = put_uint(p, n, 16, width, zero_padded);
        }
        else if (c == 'u')
        {
            unsigned int n = va_arg(args, unsigned int);
            p = put_uint(p, n, 10, width, zero_padded);
        }
        else if (c == 'd')
        {
            int n = va_arg(args, int);
            if (n < 0)
            {
                *p++ = '-';
                n = -n;
            }
            p = put_uint(p, n, 10, 0, false);
        }
        else if (c == 'A')
        {
            uint8_t *arr = va_arg(args, uint8_t*);
            int n = va_arg(args, int);
            for (int i = 0; i < n; i++)
            {
                if (i > 0)
                    *p++ = ' ';
                p = put_uint(p, arr[i], 16, 2, true);
            }
        }
    }
    *p = 0;
    return p-buf;
}
