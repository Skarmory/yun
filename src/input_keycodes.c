#include "input_keycodes.h"

#include "term.h"

#include "string.h"

static const char* c_arrow_up            = "\033[A";
static const char* c_arrow_down          = "\033[B";
static const char* c_arrow_right         = "\033[C";
static const char* c_arrow_left          = "\033[D";
static const char* c_control_arrow_up    = "\003[1;5A";
static const char* c_control_arrow_down  = "\003[1;5B";
static const char* c_control_arrow_right = "\003[1;5C";
static const char* c_control_arrow_left  = "\003[1;5D";

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
        if((buf[0] >= KEYCODE_CHAR_RANGE_START && buf[0] <= KEYCODE_CHAR_RANGE_END) || buf[0] == KEYCODE_ENTER || buf[0] == KEYCODE_ESC || buf[0] == KEYCODE_BACKSPACE)
        {
            return buf[0];
        }

        return KEYCODE_UNKNOWN;
    }

    // If buffer length is > 1 from a single char, it must be an escape key or special character of some sort
    return _handle_escape_sequence(buf);
}

