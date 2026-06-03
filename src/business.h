#ifndef BUSINESS_OCFP_H
#define BUSINESS_OCFP_H

#include <stddef.h>
#include <stdint.h>

#include "room.h"

// it
int parse_it_file_b(char **argv, size_t i);
int add_item_to_it_b(char **argv, size_t i);
int edit_item_in_it_b(char **argv, size_t i);
int remove_item_from_it_b(char **argv, size_t i);
int item_info_from_it_b(char **argv, size_t i);

// tm
int parse_tm_file_b(char **argv, size_t i);
int add_item_to_tm_b(char **argv, size_t i);
int edit_item_in_tm_b(char **argv, size_t i);
int remove_item_from_tm_b(char **argv, size_t i);

// sav
int parse_sav_file_b(char **argv, size_t i);
int reserialize_sav_file_b(char **argv, size_t i);

// hoe
int parse_hoe_file_b(char **argv, size_t i);
int reserialize_hoe_file_b(char **argv, size_t i);

room_id find_item(uint8_t *obscure_path, uint32_t uid);

int test(char **argv, size_t i);

#endif /* !BUSINESS_OCFP_H */