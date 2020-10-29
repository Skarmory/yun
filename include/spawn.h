#ifndef NAXX_SPAWN_H
#define NAXX_SPAWN_H

#include <stdbool.h>

struct ConsoleCommand;

enum SpawnType
{
    SPAWN_TYPE_UNKNOWN = -1,
    SPAWN_TYPE_MON
};

enum SpawnType spawn_type_from_string(const char* str);

bool spawn_mon(const char* id, int wx, int wy);

void init_spawn_console_command(struct ConsoleCommand* out_command);

#endif
