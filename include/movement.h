#ifndef NAXX_MOVEMENT_H
#define NAXX_MOVEMENT_H

#include "mon_attr.h"

#include <stdbool.h>

struct Mon;

bool move_is_pathable(int destx, int desty, MonAttrMoveFlags move_flags);
bool move_is_valid(int destx, int desty, MonAttrMoveFlags move_flags);
bool move_mon(struct Mon*, int newx, int newy);

#endif

