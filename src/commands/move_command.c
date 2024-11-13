#include "commands/move_command.h"

#include "commands/command.h"

struct MoveCommand
{
    struct MapComponent* map_c;
    struct PositionComponent* mover_position_c;
    int move_to_x;
    int move_to_y;
};

const struct MoveCommand* move_command_new(struct MapComponent* map_c, struct PositionComponent* pos_c, int move_to_x, int move_to_y)
{
    struct MoveCommand* mc = malloc(sizeof(struct MoveCommand));
    mc->map_c = map_c;
    mc->pos_c = pos_c;
    mc->move_to_x = move_to_x;
    mc->move_to_y = move_to_y;

    struct Command* cmd = malloc(sizeof(struct Command));
    cmd->free_func = move_command_free;
    cmd->execute_func = move_command_execute;
    cmd->data = mc;

    return cmd;
}

void move_command_execute(const struct Command* cmd);
{

}
