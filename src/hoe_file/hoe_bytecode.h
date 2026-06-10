#ifndef HOE_BYTECODE_H
#define HOE_BYTECODE_H

#include "hoe_file.h"

void pretty_print_hoe_event_bytecode(hoe_event *event);
//void pretty_print_hoe_event_bytecode_primary_structure(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation);
void print_hoelang_main(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation);

#endif /* !HOE_BYTECODE_H */