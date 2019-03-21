#ifndef NAXX_LOG_H
#define NAXX_LOG_H

#define MSGHIST 0
#define DEBUG   1

void init_logs(void);
void uninit_logs(void);
void log_msg(int logtype, char* msg);
void log_format_msg(int logtype, char* format, ...);
void log_scheck_fail(char* msg);

#endif
