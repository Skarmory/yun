#include "cursor_utils.h"

#include <stdbool.h>

enum CursorMoveCommand cursor_move(int x, int y, int* out_x, int* out_y)
{
    while(true)
    {
        enum CursorMoveCommand cmd = (enum CursorMoveCommand)get_key();
        switch(cmd)
        {
            case CURSOR_MOVE_CONFIRM:
            case CURSOR_MOVE_CANCEL:
            {
                return cmd;
            }

            case CURSOR_MOVE_UP:
            case CURSOR_MOVE_DOWN:
            case CURSOR_MOVE_LEFT:
            case CURSOR_MOVE_RIGHT:
            case CURSOR_MOVE_LEFT_UP:
            case CURSOR_MOVE_LEFT_DOWN:
            case CURSOR_MOVE_RIGHT_UP:
            case CURSOR_MOVE_RIGHT_DOWN:
            {
                int dx = 0;
                int dy = 0;
                if(cmd == CURSOR_MOVE_UP || cmd == CURSOR_MOVE_LEFT_UP || cmd == CURSOR_MOVE_RIGHT_UP)
                {
                    --dy;
                }
                else if(cmd == CURSOR_MOVE_DOWN || cmd == CURSOR_MOVE_LEFT_DOWN || cmd == CURSOR_MOVE_RIGHT_DOWN)
                {
                    ++dy;
                }

                if(cmd == CURSOR_MOVE_LEFT || cmd == CURSOR_MOVE_LEFT_UP || cmd == CURSOR_MOVE_LEFT_DOWN)
                {
                    --dx;
                }
                else if(cmd == CURSOR_MOVE_RIGHT || cmd == CURSOR_MOVE_RIGHT_UP || cmd == CURSOR_MOVE_RIGHT_DOWN)
                {
                    ++dx;
                }

                *out_x = x + dx;
                *out_y = y + dy;

                return cmd;
            }

            case CURSOR_MOVE_UNKNOWN:
            {
                break;
            }
        }
    }

    return CURSOR_MOVE_UNKNOWN;
}
