#include "gameplay_commands.h"

bool gameplay_command_is_movement(enum GameplayCommand cmd)
{
    return
           cmd == GAMEPLAY_COMMAND_MOVE_LEFT
        || cmd == GAMEPLAY_COMMAND_MOVE_RIGHT
        || cmd == GAMEPLAY_COMMAND_MOVE_UP
        || cmd == GAMEPLAY_COMMAND_MOVE_DOWN
        || cmd == GAMEPLAY_COMMAND_MOVE_LEFT_UP
        || cmd == GAMEPLAY_COMMAND_MOVE_LEFT_DOWN
        || cmd == GAMEPLAY_COMMAND_MOVE_RIGHT_UP
        || cmd == GAMEPLAY_COMMAND_MOVE_RIGHT_DOWN;
}
