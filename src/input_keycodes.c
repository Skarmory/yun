#include "input_keycodes.h"

#include "term.h"

#include "string.h"

const static char* c_arrow_up    = "\033[A";
const static char* c_arrow_down  = "\033[B";
const static char* c_arrow_right = "\033[C";
const static char* c_arrow_left  = "\033[D";

enum KeyCode _handle_escape_sequence(char buf[8])
{
    if(strcmp(buf, c_arrow_up) == 0)
    {
        return KEYCODE_ARROW_UP;
    }

    if(strcmp(buf, c_arrow_down) == 0)
    {
        return KEYCODE_ARROW_DOWN;
    }

    if(strcmp(buf, c_arrow_right) == 0)
    {
        return KEYCODE_ARROW_RIGHT;
    }

    if(strcmp(buf, c_arrow_left) == 0)
    {
        return KEYCODE_ARROW_LEFT;
    }

    return KEYCODE_UNKNOWN;
}

enum KeyCode get_key(void)
{
    char buf[8];
    term_getch(buf, 8);

    if(strlen(buf) == 1)
    {
        // Simple ASCII code to handle
        if((buf[0] >= KEYCODE_CHAR_RANGE_START && buf[0] <= KEYCODE_CHAR_RANGE_END) || buf[0] == KEYCODE_ENTER || buf[0] == KEYCODE_ESC)
        {
            return buf[0];
        }

        return KEYCODE_UNKNOWN;
    }

    // If buffer length is > 1 from a single char, it must be an escape key or special character of some sort
    return _handle_escape_sequence(buf);
}

