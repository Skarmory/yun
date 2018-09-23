#ifndef MESSAGE_H
#define MESSAGE_H

void init_msgs(void);
void display_msg(char* msg);
void display_format_msg(char* format, ...);
void clear_msgs(void);
void flush_msg_buffer(void);

#endif
