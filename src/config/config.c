#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

#define GET_LINE_SUCCESS 0
#define GET_LINE_ERROR 1
#define GET_LINE_EOF 2
#define READ_LINE_SUCCESS 0
#define READ_LINE_ERROR 1
#define READ_LINE_EOF 2

/*
** This only handles files that use LF instead of CRLF
*/

int get_config_line(FILE *file, char **next_line)
{
    size_t line_len = 0;
    char buf1[] = { 0 };
    size_t total_read = 0;
    size_t r;
    int is_newline = 0;

    do
    {
        r = fread(buf1, 1, 1, file);
        total_read += r;
        line_len++;
    } while (buf1[0] != '\n' && r != 0);

    if (r)
    {
        is_newline = 1;
    }

    if (total_read == 0)
    {
        *next_line = NULL;
        return GET_LINE_EOF;
    }

    // line_len is ahead of the length of the line by 1 because we increment it
    // regardless of the condition evaluation, so we subtract one
    line_len--;

    // line_len + 1 because we need 1 byte for the null terminating byte
    char *line = calloc(1, line_len + 1);
    if (!line)
    {
        fprintf(stderr, "ERROR: Couldn't calloc the config line\n");
        *next_line = NULL;
        return GET_LINE_ERROR;
    }

    fseek(file, -line_len - is_newline, SEEK_CUR);
    fread(line, 1, line_len, file);
    fseek(file, 1, SEEK_CUR);
    line[line_len] = 0;
    *next_line = line;
    return GET_LINE_SUCCESS;
}

int read_config_line(FILE *file, struct pair *new_pair)
{
    char *next_line = NULL;
    int get_line_status = get_config_line(file, &next_line);
    if (get_line_status == GET_LINE_ERROR)
    {
        return READ_LINE_ERROR;
    }
    else if (get_line_status == GET_LINE_EOF)
    {
        return READ_LINE_EOF;
    }

    size_t equal_sign_index = 0;
    while (next_line[equal_sign_index] && next_line[equal_sign_index] != '=')
    {
        equal_sign_index++;
    }

    if (next_line[equal_sign_index] != '=')
    {
        fprintf(stderr, "ERROR: No equal sign found, bad format\n");
        free(next_line);
        return 1;
    }

    size_t line_len = strlen(next_line);

    char *key = calloc(1, equal_sign_index + 1);
    if (!key)
    {
        fprintf(stderr, "ERROR: Couldn't calloc the key\n");
        free(next_line);
        return 1;
    }

    strncpy(key, next_line, equal_sign_index);
    key[equal_sign_index] = 0;

    size_t value_len = line_len - equal_sign_index - 1;
    char *value = calloc(1, value_len + 1);
    if (!value)
    {
        fprintf(stderr, "ERROR: Couldn't calloc the value\n");
        free(next_line);
        free(key);
        return 1;
    }

    // this time we add 1 to value_len to copy the null terminating byte too
    strncpy(value, next_line + equal_sign_index + 1, value_len + 1);

    new_pair->key = key;
    new_pair->value = value;
    free(next_line);
    return 0;
}

config *read_config(char *path)
{
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        fprintf(stderr, "ERROR: Couldn't read config file %s\n", path);
        return NULL;
    }

    config *result = calloc(1, sizeof(config));
    if (!result)
    {
        fclose(file);
        return NULL;
    }

    struct pair *pairs = NULL;
    struct pair new_pair = { 0 };
    size_t pairs_count = 0;
    int rls = 1;

    while ((rls = read_config_line(file, &new_pair)) == READ_LINE_SUCCESS)
    {
        pairs_count++;
        struct pair *pairs2 = realloc(pairs, pairs_count * sizeof(struct pair));
        if (!pairs2)
        {
            fprintf(stderr, "ERROR: Couldn't realloc pairs\n");
            // TODO: free everything
            return NULL;
        }

        pairs = pairs2;
        pairs[pairs_count - 1] = new_pair;
    }

    if (rls == READ_LINE_ERROR)
    {
        fprintf(stderr, "ERROR: Error while reading line\n");
        // TODO: free everything
        return NULL;
    }

    result->pairs = pairs;
    result->pairs_count = pairs_count;
    fclose(file);
    return result;
}

void free_pair(struct pair *pair)
{
    free((char*)pair->key);
    free(pair->value);
}

void free_config(config *config)
{
    size_t pairs_count = config->pairs_count;
    struct pair *pairs = config->pairs;

    for (size_t i = 0; i < pairs_count; i++)
    {
        free_pair(pairs + i);
    }

    free(pairs);
    free(config);
}

char *get_config_value(config *config, char *key)
{
    struct pair *p = config->pairs;
    size_t pairs_count = config->pairs_count;

    for (size_t i = 0; i < pairs_count; i++)
    {
        if (strcmp(p[i].key, key) == 0)
        {
            return p[i].value;
        }
    }

    return NULL;
}

int is_key_in_config(config *config, char *key)
{
    struct pair *p = config->pairs;
    size_t pairs_count = config->pairs_count;

    for (size_t i = 0; i < pairs_count; i++)
    {
        if (strcmp(p[i].key, key) == 0)
        {
            return 1;
        }
    }

    return 0;
}

void print_pair(struct pair *pair)
{
    printf("<%s> = <%s>\n", pair->key, pair->value);
}

void print_config(config *config)
{
    size_t pairs_count = config->pairs_count;
    struct pair *pairs = config->pairs;

    for (size_t i = 0; i < pairs_count; i++)
    {
        print_pair(pairs + i);
    }
}
