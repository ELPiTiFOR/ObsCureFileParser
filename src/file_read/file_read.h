#ifndef FILE_READ_H
#define FILE_READ_H

#include <stdint.h>
#include <stdio.h>

#include "utils.h"

// reads the next 4 bytes in the file interpreting them as LSB
// if the next 4 bytes are `AA BB CC DD` the result will be `DD CC BB AA`
uint32_t read_4byte_lsb(FILE *file, size_t *read);

// reads the next 4 bytes in the file interpreting them as MSB
// if the next 4 bytes are `AA BB CC DD` the result will be `AA BB CC DD`
uint32_t read_4byte_msb(FILE *file, size_t *read);

uint16_t read_2byte_lsb(FILE *file, size_t *read);

uint8_t read_1byte(FILE *file, size_t *read);

float get_first_number_f(FILE *file);
float get_first_number_f_lsb(FILE *file);

char *str_from_file(char *src, size_t *total_written);

#endif /* !FILE_READ_H */