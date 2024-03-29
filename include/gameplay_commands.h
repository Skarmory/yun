#ifndef YUN_GAMEPLAY_COMMANDS_H
#define YUN_GAMEPLAY_COMMANDS_H

#include <scieppend/core/input_keycodes.h>

#include <stdbool.h>

enum GameplayCommand
{
    GAMEPLAY_COMMAND_CAST_SPELL               = KEYCODE_C,
    GAMEPLAY_COMMAND_DISPLAY_CHARACTER_SCREEN = KEYCODE_c,
    GAMEPLAY_COMMAND_DISPLAY_INVENTORY        = KEYCODE_i,
    GAMEPLAY_COMMAND_DISPLAY_POSITION         = KEYCODE_AT,
    GAMEPLAY_COMMAND_LOOK                     = KEYCODE_SEMICOLON,
    GAMEPLAY_COMMAND_MOVE_LEFT                = KEYCODE_h,
    GAMEPLAY_COMMAND_MOVE_RIGHT               = KEYCODE_l,
    GAMEPLAY_COMMAND_MOVE_UP                  = KEYCODE_k,
    GAMEPLAY_COMMAND_MOVE_DOWN                = KEYCODE_j,
    GAMEPLAY_COMMAND_MOVE_LEFT_UP             = KEYCODE_y,
    GAMEPLAY_COMMAND_MOVE_LEFT_DOWN           = KEYCODE_b,
    GAMEPLAY_COMMAND_MOVE_RIGHT_UP            = KEYCODE_u,
    GAMEPLAY_COMMAND_MOVE_RIGHT_DOWN          = KEYCODE_n,
    GAMEPLAY_COMMAND_PASS_TURN                = KEYCODE_DOT,
    GAMEPLAY_COMMAND_PICK_UP                  = KEYCODE_COMMA,
    GAMEPLAY_COMMAND_NO_SAVE_AND_QUIT         = KEYCODE_Q,
    GAMEPLAY_COMMAND_SAVE_AND_QUIT            = KEYCODE_q,
    GAMEPLAY_COMMAND_SHOW_CONSOLE             = KEYCODE_BACKTICK
};

bool gameplay_command_is_movement(enum GameplayCommand cmd);

#endif
