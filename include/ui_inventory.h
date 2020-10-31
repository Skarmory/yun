#ifndef YUN_INVENTORY_UI_H
#define YUN_INVENTORY_UI_H

#include <stdbool.h>

struct Mon;

bool display_inventory_player(void);

void display_inventory_read_only(struct Mon* mon);

#endif
