#ifndef YUN_LOOK_H
#define YUN_LOOK_H

#include <scieppend/core/input_keycodes.h>
#include <scieppend/core/symbol.h>

struct MapLocation;
struct Mon;

struct Symbol look_get_symbol(const struct MapLocation* loc, const struct Mon* looker);
void look(void);

#endif
