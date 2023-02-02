#include "console.h"

#include "message.h"

#include <scieppend/core/colour.h>
#include <scieppend/core/input_keycodes.h>
#include <scieppend/core/term.h>

#include <stdio.h>
#include <string.h>

#define INPUT_BUFFER_LENGTH MSGBOX_W
#define INPUT_TEXT_MAX (INPUT_BUFFER_LENGTH-1)
#define CURSOR_POS_MAX (INPUT_TEXT_MAX-1)

enum ConsoleTextEditorCommand
{
    CONSOLE_TEXT_EDITOR_COMMAND_QUIT        = KEYCODE_ESC,
    CONSOLE_TEXT_EDITOR_COMMAND_EXECUTE     = KEYCODE_ENTER,
    CONSOLE_TEXT_EDITOR_COMMAND_BACK_ONE    = KEYCODE_ARROW_LEFT,
    CONSOLE_TEXT_EDITOR_COMMAND_FORWARD_ONE = KEYCODE_ARROW_RIGHT,
    CONSOLE_TEXT_EDITOR_COMMAND_BACK_DELETE = KEYCODE_BACKSPACE
};

struct _ConsoleTextEditorState
{
    int cursor_pos;
    int length;
    char input[INPUT_BUFFER_LENGTH];
};

static void _move_cursor_back_one_space(struct _ConsoleTextEditorState* state)
{
    if(state->length == 0 || state->cursor_pos == 0)
    {
        return;
    }

    --state->cursor_pos;
}

static void _move_cursor_forward_one_space(struct _ConsoleTextEditorState* state)
{
    if(state->length == 0 || state->cursor_pos == CURSOR_POS_MAX || state->cursor_pos == state->length)
    {
        return;
    }

    ++state->cursor_pos;
}

static void _add_character_to_input(struct _ConsoleTextEditorState* state, enum KeyCode key)
{
    if(state->cursor_pos == INPUT_TEXT_MAX || state->length == INPUT_TEXT_MAX)
    {
        return;
    }

    if(key >= KEYCODE_CHAR_RANGE_START && key <= KEYCODE_CHAR_RANGE_END)
    {
        char tmp[INPUT_BUFFER_LENGTH];
        memset(tmp, '\0', INPUT_BUFFER_LENGTH);

        snprintf(tmp, INPUT_BUFFER_LENGTH, "%s", state->input + state->cursor_pos);
        snprintf(state->input + state->cursor_pos + 1, INPUT_BUFFER_LENGTH - state->cursor_pos, "%s", tmp);
        state->input[state->cursor_pos] = (char)key;

        ++state->length;

        if(state->cursor_pos != CURSOR_POS_MAX)
        {
            ++state->cursor_pos;
        }
    }
}

static void _back_delete_one(struct _ConsoleTextEditorState* state)
{
    if(state->cursor_pos == 0 || state->length == 0)
    {
        return;
    }

    char tmp[INPUT_BUFFER_LENGTH];
    memset(tmp, '\0', INPUT_BUFFER_LENGTH);

    snprintf(tmp, INPUT_BUFFER_LENGTH, "%s", state->input + state->cursor_pos);
    snprintf(state->input + state->cursor_pos - 1, INPUT_BUFFER_LENGTH - state->cursor_pos, "%s", tmp);
    state->input[state->length] = '\0';
    --state->length;
    --state->cursor_pos;
}

bool console_input(char* out_command)
{
    struct _ConsoleTextEditorState state;
    state.cursor_pos = 0;
    state.length = 0;
    memset(&state.input, '\0', INPUT_BUFFER_LENGTH);

    clear_msgs();

    while(true)
    {
        term_clear_area(MSGBOX_X, MSGBOX_Y, MSGBOX_W, MSGBOX_H);
        term_draw_text(MSGBOX_X, MSGBOX_Y, &g_colours[CLR_WHITE], &g_colours[CLR_DEFAULT], A_NONE_BIT, "Input console command:");
        term_draw_ftext(MSGBOX_X, MSGBOX_Y+1, &g_colours[CLR_WHITE], &g_colours[CLR_DEFAULT], A_NONE_BIT, "%s ", state.input);
        term_draw_ftext(MSGBOX_X, MSGBOX_Y+2, &g_colours[CLR_WHITE], &g_colours[CLR_DEFAULT], A_NONE_BIT, "Length: %d, Cursor Pos: %d", state.length, state.cursor_pos);
        term_draw_symbol(MSGBOX_X + state.cursor_pos, MSGBOX_Y+1, &g_colours[CLR_BLACK], &g_colours[CLR_WHITE], A_NONE_BIT, state.input[state.cursor_pos] == '\0' ? ' ' : state.input[state.cursor_pos]);
        term_refresh();

        enum KeyCode key = get_key();
        switch(key)
        {
            case CONSOLE_TEXT_EDITOR_COMMAND_QUIT:
            {
                return true;
            }
            case CONSOLE_TEXT_EDITOR_COMMAND_EXECUTE:
            {
                snprintf(out_command, INPUT_BUFFER_LENGTH, "%s", state.input);
                return false;
            }
            case CONSOLE_TEXT_EDITOR_COMMAND_BACK_ONE:
            {
                _move_cursor_back_one_space(&state);
                break;
            }
            case CONSOLE_TEXT_EDITOR_COMMAND_FORWARD_ONE:
            {
                _move_cursor_forward_one_space(&state);
                break;
            }
            case CONSOLE_TEXT_EDITOR_COMMAND_BACK_DELETE:
            {
                _back_delete_one(&state);
                break;
            }
            default:
            {
                _add_character_to_input(&state, key);
                break;
            }
        }
    }
}
