#ifndef BUSINESS_H
#define BUSINESS_H

#include <stddef.h>

// it
int add_item_to_it_b(char **argv, size_t i);
int remove_item_from_it_b(char **argv, size_t i);

// tm
int parse_tm_file_b(char **argv, size_t i);

#endif /* !BUSINESS_H */