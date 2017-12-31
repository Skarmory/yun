#ifndef MESSAGE_H
#define MESSAGE_H

#define MSGBOX_X 0
#define MSGBOX_Y 40
#define MSGBOX_W 80
#define MSGBOX_H 3

void display_msg(char* msg);
void clear_msgs(void);
void flush_msg_buffer(void);

#endif
