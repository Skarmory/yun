#ifndef YUN_COMMANDS_MOVE_COMMAND_H
#define YUN_COMMANDS_MOVE_COMMAND_H

struct MapComponent;
struct PositionComponent;

const struct Command* move_command_new(struct MapComponent* map_c, struct PositionComponent* pos_c, int move_to_x, int move_to_y);
void move_command_free(struct Command* cmd);

void move_command_execute(const struct Command* cmd);

#endif
