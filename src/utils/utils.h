#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

// we assume buf is 4 bytes long
void fill_buf_uint32_msb(uint32_t u, char *buf);
// we assume buf is 4 bytes long
void fill_buf_uint32_lsb(uint32_t u, char *buf);

void fill_buf_uint16_lsb(uint16_t u, char *buf);

void fill_buf_float_msb(float f, char *buf);

uint32_t my_atoi_base(char *str, int base);

uint32_t buf_to_long(uint8_t *buf);

char *duplicate_string(char *str);
char *duplicate_string_until(char *str, int c, size_t *index);

float buf_to_float(uint8_t *buf, size_t len);
float str_to_float(uint8_t *str, size_t len);

void print_time(uint32_t time);

void print_hexdump(uint8_t *content, size_t len, size_t indent);

#endif /* !UTILS_H */