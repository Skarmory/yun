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

/* Write a message to the messages area */
void display_msg(char* msg)
{
    _write_msg_log(msg);
    mvprintw(MSGBOX_Y, 0, msg);    
}

void clear_msgs(void)
{
    char buf[MSGBOX_W];
    memset(buf, ' ', MSGBOX_W-1);
    mvprintw(MSGBOX_Y, MSGBOX_X, buf);
}
