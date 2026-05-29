#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t uint32_t_revert(uint32_t u)
{
    uint32_t res = 0;
    for (size_t i = 0; i < 4; i++)
    {
        res = res << 8;
        res |= u & 0xFF;
        u = u >> 8;
    }

    return res;
}

void fill_buf_uint32_msb(uint32_t u, char *buf)
{
    uint32_t u_lsb = uint32_t_revert(u);

    void *buf_v = buf;
    uint32_t *buf_u = buf_v;
    buf_u[0] = u_lsb;
}

void fill_buf_uint32_lsb(uint32_t u, char *buf)
{
    void *buf_v = buf;
    uint32_t *buf_u = buf_v;
    buf_u[0] = u;
}

void fill_buf_uint16_lsb(uint16_t u, char *buf)
{
    void *buf_v = buf;
    uint16_t *buf_u = buf_v;
    buf_u[0] = u;
}

void fill_buf_float_msb(float f, char *buf)
{
    void *f_v = &f;
    uint8_t *f_c = f_v;

    for (size_t i = 0; i < 4; i++)
    {
        buf[i] = f_c[3 - i];
    }
}

int get_value_from_char_base(char c, int base)
{
    /*
    char buf_log[64];
    sprintf(buf_log, "Parsing digit <%c> in base %d\n", c, base);
    log(LOG_VERY_MINOR, buf_log);
    */

    int res = 0;

    if (c >= '0' && c <= '9')
    {
        res = c - '0';
    }
    else if (c >= 'a' && c <= 'z')
    {
        res = c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'Z')
    {
        res = c - 'A' + 10;
    }
    else
    {
        return -1;
    }

    if (res >= base)
    {
        /*
        char buf_log[64];
        sprintf(buf_log, "%c is an nvalid digit in %d base\n", c, base);
        log(ERROR, buf_log);
        */
        return -1;
    }

    return res;
}

uint32_t my_atoi_base(char *str, int base)
{
    //printf("Parsing <%s> in base %d\n", str, base);
    uint32_t res = 0;
    size_t i = 0;
    char c = 0;
    while ((c = str[i]))
    {
        res *= base;
        int value = get_value_from_char_base(c, base);
        if (value < 0)
        {
            fprintf(stderr, "ERROR: The given string is not valid");
            return 0;
        }

        res += value;
        i++;
    }

    return res;
}

uint32_t buf_to_long(uint8_t *buf)
{
    uint32_t res = 0;
    //printf("res = %d\n", res);
    for (size_t i = 0; i < 4; i++)
    {
        res = res << 8;
        res |= buf[i];
        //printf("res = %d\n", res);
    }

    return res;
}

// returns a new heap allocated string equal in content to `str`
char *duplicate_string(char *str)
{
    size_t len = strlen(str);
    char *res = calloc(len + 1, 1);
    if (!res)
    {
        fprintf(stderr, "Couldn't calloc() %s\n", str);
        return NULL;
    }

    strcpy(res, str);
    return res;
}

char *my_strchrnul(char *str, int c)
{
   while(*str && *str != c)
   {
      str++;
   }

   return str;
}

char *duplicate_string_until(char *str, int c, size_t *index)
{
    char *mark = my_strchrnul(str, c);
    if (!mark)
    {
        /*
        char buf_log[512];
        sprintf(buf_log, "Character %c not found in %s\n", c, str);
        log(ERROR, buf_log);
        */
        return NULL;
    }
    if (*mark == '\n' && *(mark - 1) == '\r')
    {
        *(mark - 1) = 0;
    }

    *mark = 0;
    char *key = duplicate_string(str);
    if (!key)
    {
        *mark = c;
        return NULL;
    }

    if (index)
    {
        *index += mark - str;
    }

    if (c == '\n')
    {
        *(mark - 1) = '\r';
    }
    *mark = c;
    return key;
}

float buf_to_float(uint8_t *buf, size_t len)
{
    if (len != 4)
    {
        fprintf(stderr, "ERROR: given buf is not 4 bytes long, can't convert to float\n");
        return 0.0;
    }

    uint8_t buf_lsb[4];
    for (size_t i = 0; i < 4; i++)
    {
        buf_lsb[i] = buf[3 - i];
    }
    /*
    for (size_t i = 0; i < 4; i++)
    {
        printf("buf_to_float: buf_lsb[%zu] = %02X\n", i, buf_lsb[i]);
    }
    */

    void *buf_lsb_v = buf_lsb;
    float *buf_lsb_f = buf_lsb_v;
    return *buf_lsb_f;
}

float str_to_float(uint8_t *str, size_t len)
{
    if (len != 8)
    {
        fprintf(stderr, "ERROR: given str is not 8 chars long (%zu | %s), can't convert to float\n", len, str);
        return 0.0;
    }

    uint8_t buf_lsb[4];
    for (size_t i = 0; i < 4; i++)
    {
        buf_lsb[3 - i] = get_value_from_char_base(str[i * 2], 16) * 16 + get_value_from_char_base(str[i * 2 + 1], 16);
    }

    for (size_t i = 0; i < 4; i++)
    {
        printf("str_to_float: buf_lsb[%zu] = %02X\n", i, buf_lsb[i]);
    }


    void *buf_lsb_v = buf_lsb;
    float *buf_lsb_f = buf_lsb_v;
    //printf("INFO: given str is %zu chars long (%s | %f)\n", len, str, *buf_lsb_f);
    return *buf_lsb_f;
}

void print_time(uint32_t time)
{
    uint32_t hours = time / 3600000;
    time -= hours * 3600000;
    uint32_t minutes = time / 60000;
    time -= minutes * 60000;
    uint32_t seconds = time / 1000;
    time -= seconds * 1000;
    printf("%02u:%02u:%02u.%03u\n", hours, minutes, seconds, time);
}

int is_basic_printable(uint8_t c)
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || (c >= '0' && c <= '9')
        || c == '_'
        || c == '/';
}

void print_hexdump(uint8_t *content, size_t len, size_t indent)
{
    for (size_t i = 0; i < len; i++)
    {
        if (i % 16 == 0)
        {
            for (size_t n = 0; n < indent; n++)
            {
                printf(" ");
            }
        }

        if (is_basic_printable(content[i]))
        {
            printf("%2c ", content[i]);
        }
        else
        {
            printf("%02X ", content[i]);
        }

        if ((i + 1) % 16 == 0 || i == len - 1)
        {
            putchar('\n');
        }
    }
}

size_t search_in_array(uint8_t *array, size_t len_a, uint8_t *pattern,
    size_t len_p)
{
    for (size_t i = 0; i < len_a; i++)
    {
        if (memcmp(array + i, pattern, len_p) == 0)
        {
            return i;
        }
    }

    return len_a;
}

uint32_t lsb_32(uint32_t number)
{
    uint8_t buf[4];
    buf[0] = (number & 0xff000000) >> 3*8;
    buf[1] = (number & 0x00ff0000) >> 2*8;
    buf[2] = (number & 0x0000ff00) >> 8;
    buf[3] = number & 0x000000ff;

    void *buf_v = buf;
    uint32_t *buf_u = buf_v;
    return *buf_u;
}