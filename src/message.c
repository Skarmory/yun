#include "message.h"
#include <ncurses.h>
#include <string.h>
#include "util.h"

void _write_msg_log(char* msg)
{
    FILE* log = fopen("msghist.log", "a");
    fprintf(log, "%s\n", msg); 
    fclose(log);
}

char msgbuf[MSGBOX_W];
int msgbuf_size = 0;

/* Write a message to the messages area */
void display_msg(char* msg)
{
    _write_msg_log(msg);

    int msgsize = 0;
    char* c = msg;
    while(*c != '\0')
    {
        msgsize++;
        c++;
    }

    if(msgbuf_size == 0)
        sprintf(msgbuf, "%s", msg);
    else if(msgbuf_size + msgsize < MSGBOX_W)
        sprintf(msgbuf, "%s %s", msgbuf, msg);

    msgbuf_size += msgsize;
}

void clear_msgs(void)
{
    char clearbuf[MSGBOX_W];
    memset(clearbuf, ' ', MSGBOX_W-1);
    mvprintw(MSGBOX_Y, MSGBOX_X, clearbuf);
}

void flush_msg_buffer(void)
{
    mvprintw(MSGBOX_Y, MSGBOX_X, msgbuf);
    msgbuf_size = 0;
    memset(msgbuf, ' ', MSGBOX_W-1);
}
