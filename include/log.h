#ifndef LOG_H
#define LOG_H

#define MSGHIST 0
#define DEBUG   1

void init_logs(void);
void log_msg(char* msg, int logtype);
void log_format_msg(char* format, int logtype, ...);
void destroy_logs(void);

#endif
