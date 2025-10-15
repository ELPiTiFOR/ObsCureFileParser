#ifndef FILE_WRITE_H
#define FILE_WRITE_H

#include <stdint.h>
#include <stdio.h>

// writes the integer 4 bytes in the file as LSB
// if the given integer is `AA BB CC DD` the result will be `DD CC BB AA`
void write_4byte_lsb(FILE *file, uint32_t u);

// writes the integer 4 bytes in the file as MSB
// if the given integer is `AA BB CC DD` the result will be `AA BB CC DD`
void write_4byte_msb(FILE *file, uint32_t u);

void write_4byte_float_msb(FILE *file, float f);

void write_array(FILE *file, uint8_t *arr, uint32_t len);

#endif /* !FILE_WRITE_H */