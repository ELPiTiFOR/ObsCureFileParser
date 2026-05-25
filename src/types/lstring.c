#include "lstring.h"

#include <stdlib.h>

void make_lstring(lstring *res, uint32_t len, uint8_t *str)
{
    // TODO: what if it fails?
    uint8_t *content = calloc(len, 1);

    for (size_t i = 0; i < len; i++)
    {
        content[i] = str[i];
    }

    res->length = len;
    res->content = content;
}

void free_lstring_content(lstring *res)
{
    free(res->content);
}