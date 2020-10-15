#ifndef NAXX_MESSAGE_H
#define NAXX_MESSAGE_H

#include <stdbool.h>

#define display_msg_log(msg)   (display_msg(true, (msg)))
#define display_msg_nolog(msg) (display_msg(false, (msg)))

#define display_fmsg_log(format, ...)   (display_fmsg(true, (format), __VA_ARGS__))
#define display_fmsg_nolog(format, ...) (display_fmsg(false, (format), __VA_ARGS__))

void display_msg(bool should_log, const char* msg);
void display_fmsg(bool should_log, const char* format, ...);
void clear_msgs(void);
void flush_msg_buffer(void);

#endif
