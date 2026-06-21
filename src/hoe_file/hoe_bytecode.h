#ifndef HOE_BYTECODE_H
#define HOE_BYTECODE_H

#include "hoe_file.h"

int print_hoelang_main(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation);
void print_hoelang_mask(hoe_event *event, uint8_t *bytecode, size_t *i, int indentation);

#endif /* !HOE_BYTECODE_H */