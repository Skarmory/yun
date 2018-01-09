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
int msgbuf_size = 0;

void _flush_and_prompt(void)
{
    flush_msg_buffer();
    mvprintw(MSGBOX_Y+1, MSGBOX_X, "-- more --");
    getch();
}

int _msg_size(char* msg)
{
    int msgsize = 0;
    char* c = msg;
    while(*c != '\0')
    {
        msgsize++;
        c++;
    }

    return msgsize; 
}

/* Write a message to the messages area */
void display_msg(char* msg)
{
    logmsg(msg, MSGHIST);

    int msgsize = _msg_size(msg);
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
            sprintf(msgbuf, "%s %s", msgbuf, msg);
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

            if(idx == -1)
            {
                if(msgbuf_size == 0)
                {
                    memcpy(msgbuf, msg, MSGBOX_W-1);
                    msgbuf[MSGBOX_W] = '\0';
                    msgbuf_size = MSGBOX_W-1;
                    msgsize -= MSGBOX_W-1;

                    msg = &msg[MSGBOX_W-1]; 
                }

                _flush_and_prompt();
                continue;
            }

            char substr[idx+1];
            memcpy(substr, msg, idx);
            substr[idx] = '\0';

            sprintf(msgbuf, "%s %s", msgbuf, substr);
            msgbuf_size = idx;
            msgsize -= idx;

            _flush_and_prompt();
            
            idx++;
            msgsize--;

            msg = &msg[idx];
        }
    }
}

void clear_msgs(void)
{
    char clearbuf[MSGBOX_W];
    memset(clearbuf, ' ', MSGBOX_W-1);
    mvprintw(MSGBOX_Y, MSGBOX_X, clearbuf);
    mvprintw(MSGBOX_Y+1, MSGBOX_X, clearbuf);
}

void flush_msg_buffer(void)
{
    mvprintw(MSGBOX_Y, MSGBOX_X, msgbuf);
    msgbuf_size = 0;
    memset(msgbuf, ' ', MSGBOX_W-1);
    msgbuf[0] = '\0';
}

