#ifndef SAV_UTILS_H
#define SAV_UTILS_H

#include <stdint.h>

#include "item.h"
#include "document_id.h"

void print_room_id(uint8_t room_id);
void print_item_loc(uint32_t item_loc, item_type id);
void print_extra_info_doc(uint32_t extra_info);
void print_extra_info_map(uint32_t extra_info);

#endif /* !SAV_UTILS_H */