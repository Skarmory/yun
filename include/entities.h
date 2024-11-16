#ifndef YUN_ENTITIES_H
#define YUN_ENTITIES_H

#include <scieppend/core/ecs.h>

struct Symbol;

void add_position_component(EntityHandle e_h, const int x, const int y, const int z);
void add_gfx_component(EntityHandle e_h, const struct Symbol* terrain);
void add_terrain_component(EntityHandle e_h, int terrain_id);
void add_feature_component(EntityHandle e_h, int feature_id);

#endif

