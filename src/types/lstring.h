#ifndef LSTRING_H
#define LSTRING_H

#include <stdint.h>

typedef struct
{
    uint32_t length;
    uint8_t *content;       // NO null terminating byte
} lstring;

void free_lstring_content(lstring *res);

#endif /* !LSTRING_H */