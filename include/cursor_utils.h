#ifndef YUN_CURSOR_UTILS_H
#define YUN_CURSOR_UTILS_H

#include <scieppend/core/input_keycodes.h>

enum CursorMoveCommand
{
    CURSOR_MOVE_UNKNOWN    = -1,
    CURSOR_MOVE_CONFIRM    = KEYCODE_ENTER,
    CURSOR_MOVE_CANCEL     = KEYCODE_ESC,
    CURSOR_MOVE_UP         = KEYCODE_k,
    CURSOR_MOVE_DOWN       = KEYCODE_j,
    CURSOR_MOVE_LEFT       = KEYCODE_h,
    CURSOR_MOVE_RIGHT      = KEYCODE_l,
    CURSOR_MOVE_LEFT_UP    = KEYCODE_y,
    CURSOR_MOVE_LEFT_DOWN  = KEYCODE_b,
    CURSOR_MOVE_RIGHT_UP   = KEYCODE_u,
    CURSOR_MOVE_RIGHT_DOWN = KEYCODE_n
};

/*
 * Logic for moving the cursor a valid move in any direction.
 * Is not a sub-state: designed to be sequentially called and specific logic implemented by the caller performed at each valid move.
 *
 * Params:
 *      in_loc: Starting location for the free move
 *      out_loc: Ending location for the free move
 *
 * Return:
 *      The *valid* keycode that the user last input
 */
enum CursorMoveCommand cursor_move(int x, int y, int* out_x, int* out_y);

#endif

