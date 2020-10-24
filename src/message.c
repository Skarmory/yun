#include "message.h"

#include "log.h"
#include "map.h"
#include "term.h"
#include "ui.h"
#include "util.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

char msgbuf[MSGBOX_W];
int msgbuf_size = 0;

/**
 * Flushes current message buffer to the display and waits for user to give input
 */
void _flush_and_prompt(void)
{
    flush_msg_buffer();
    term_draw_text(MSGBOX_X, MSGBOX_Y+1, NULL, NULL, 0, "-- more--");

    // Redraw visual info
    // This will show the partial updates for the turn up to this prompt
    display_main_screen();

    term_wait_on_input();
    clear_msgs();
}

/**
 * Write a message to the messages area
 */
void display_msg(bool should_log, const char* msg)
{
    if(should_log)
        log_msg(MSGHIST, msg);

    int msgsize = strlen(msg);
    bool handled = false;

    while(!handled)
    {
        if(msgbuf_size == 0 && msgsize < MSGBOX_W)
        {
            // First msg in buffer and msg can fit in buffer
            sprintf(msgbuf, "%s", msg);
            msgbuf_size += msgsize;
            handled = true;
        }
        else if((msgbuf_size + msgsize + 1) < MSGBOX_W) // Plus 1 to account for the space
        {
            // Compound multiple messages, so long as they fit in the buffer
            strcat(msgbuf, " ");
            strcat(msgbuf, msg);
            msgbuf_size += (msgsize + 1);
            handled = true;
        }
        else
        {
            // handle messages that are larger than the display area
            // by splitting it up and prompting user to input to show
            int idx = (MSGBOX_W - 1) - msgbuf_size - 1;
            while(!isspace(msg[idx]) && idx >= 0)
                idx--;

            if(idx == -1) // This string is somehow larger than the display area
            {
                if(msgbuf_size == 0)
                {
                    // Copy as much of the message as possible into the buffer
                    memcpy(msgbuf, msg, MSGBOX_W-1);
                    msgbuf[MSGBOX_W-1] = '\0';
                    msgbuf_size = MSGBOX_W-1;
                    msgsize -= MSGBOX_W-1;

                    // Push msg start pointer to the next position after the part copied into the buffer
                    msg = &msg[MSGBOX_W-1];
                }

                _flush_and_prompt();
                continue;
            }

            char substr[idx+1];
            memcpy(substr, msg, idx);
            substr[idx] = '\0';

            // Don't add a leading space to the message
            if(msgbuf_size > 0)
                strcat(msgbuf, " ");
            strcat(msgbuf, substr);
            msgbuf_size = idx;
            msgsize -= idx;

            _flush_and_prompt();

            idx++;
            msgsize--;

            msg = &msg[idx];
        }
    }
}

/**
 * Displays a message using a standard C format string and arguments
 */
void display_fmsg(bool should_log, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    char msg[256];
    vsnprintf(msg, 256, format, args);
    display_msg(should_log, msg);

    va_end(args);
}

/**
 * Blanks the message area of the UI
 */
void clear_msgs(void)
{
    term_clear_area(MSGBOX_X, MSGBOX_Y, MSGBOX_W, MSGBOX_H);
    term_refresh();
}

/**
 * Write message buffer to the message area of the UI, the clears the message buffer
 */
void flush_msg_buffer(void)
{
    term_draw_text(MSGBOX_X, MSGBOX_Y, NULL, NULL, 0, msgbuf);
    term_refresh();
    msgbuf_size = 0;
    memset(msgbuf, '\0', MSGBOX_W-1);
}
