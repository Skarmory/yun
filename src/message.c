#include "message.h"
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "util.h"
#include "log.h"

#define MSGBOX_X 0
#define MSGBOX_Y 40
#define MSGBOX_W 80
#define MSGBOX_H 3

char msgbuf[MSGBOX_W];
char clearbuf[MSGBOX_W];
int msgbuf_size = 0;

/**
 * Initialises the clear buf
 */
void init_msgs(void)
{
    memset(clearbuf, ' ', MSGBOX_W-1);
}

/**
 * Flushes current message buffer to the display and waits for user to give input
 */
void _flush_and_prompt(void)
{
    flush_msg_buffer();
    mvprintw(MSGBOX_Y+1, MSGBOX_X, "-- more --");
    getch();
}

/**
 * Write a message to the messages area
 */
void display_msg(char* msg)
{
    log_msg(msg, MSGHIST);

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
                    msgbuf[MSGBOX_W] = '\0';
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
void display_format_msg(char* format, ...)
{
    va_list args;
    va_start(args, format);

    char msg[256];
    vsnprintf(msg, 256, format, args);
    display_msg(msg);

    va_end(args);
}

/**
 * Blanks the message area of the UI
 */
void clear_msgs(void)
{
    mvprintw(MSGBOX_Y, MSGBOX_X, clearbuf);
    mvprintw(MSGBOX_Y+1, MSGBOX_X, clearbuf);
}

/**
 * Write message buffer to the message area of the UI, the clears the message buffer
 */
void flush_msg_buffer(void)
{
    mvprintw(MSGBOX_Y, MSGBOX_X, msgbuf);
    msgbuf_size = 0;
    memset(msgbuf, ' ', MSGBOX_W-1);
    msgbuf[0] = '\0';
}

