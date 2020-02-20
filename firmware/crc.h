/*
    CRC32 module
*/

#ifndef CRC_H
#define CRC_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

 uint32_t crc32_no_table(const unsigned char *data, unsigned int size);
 uint16_t crc16_iso13239_no_table(const unsigned char *data, unsigned int size);

#ifdef __cplusplus
}
#endif

#endif // CRC_H
