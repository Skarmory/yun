#ifndef YUN_GAME_WORLD_H
#define YUN_GAME_WORLD_H

struct ECSWorld;

struct ECSWorld* game_world_get(void);

void game_world_init(void);
void game_world_uninit(void);

#endif

