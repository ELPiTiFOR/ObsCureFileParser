#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

// we assume buf is 4 bytes long
void fill_buf_uint32_msb(uint32_t u, char *buf);
// we assume buf is 4 bytes long
void fill_buf_uint32_lsb(uint32_t u, char *buf);

uint32_t my_atoi_base(char *str, int base);

#endif /* !UTILS_H */