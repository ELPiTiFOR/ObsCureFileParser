#ifndef HOE_EVENT_H
#define HOE_EVENT_H

#include <stdio.h>

#include "hoe_file.h"

int parse_hoe_event(hoe_file *hoe, FILE *file);
void print_hoe_event(hoe_event *event);
int serialize_hoe_event(hoe_event *event, FILE *file);
void free_hoe_event(hoe_event *event);
void fill_it_args(hoe_file *hoe);

void print_hoe_uk_ints(hoe_event *event);
int pretty_print_hoe_event_bytecode(hoe_event *event);

#endif /* !HOE_EVENT_H */