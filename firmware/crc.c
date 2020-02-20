#include "crc.h"

uint32_t crc32_no_table(const unsigned char *data, unsigned int size)
{
   int i, j;
   uint32_t res;

   res = 0xffffffff;
   for (i = 0; i < size; ++i)
   {
      res = res ^ data[i];
      for (j = 7; j >= 0; --j)
         res = (res >> 1) ^ (0xedb88320 & (-(res & 1)));
   }
   return ~res;
}

uint16_t crc16_iso13239_no_table(const unsigned char *data, unsigned int size)
{
    int i, j;
    uint16_t res = 0xffff;
    for (i = 0; i < size; ++i)
    {
         res = res ^ data[i];
         for (j = 7; j >= 0; --j)
            res = (res >> 1) ^ (0x8408 & (-(res & 1)));
     }
     return ~res;
}
