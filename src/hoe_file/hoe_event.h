#ifndef HOE_EVENT_H
#define HOE_EVENT_H

#include <stdio.h>

#include "hoe_file.h"

int parse_hoe_event(hoe_file *hoe, FILE *file);
void print_hoe_event(hoe_event *event);
void free_hoe_event(hoe_event *event);

#endif /* !HOE_EVENT_H */