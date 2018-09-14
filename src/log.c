#include "log.h"
#include <stdio.h>
#include <stdarg.h>

const char* MSGHIST_FNAME = "msghist.log"; 
const char* DEBUGLOG_FNAME = "debug.log"; 

FILE* msghist_file;
FILE* debug_file;

void init_logs(void)
{
    msghist_file = fopen(MSGHIST_FNAME, "w");
    debug_file = fopen(DEBUGLOG_FNAME, "w");
}

void log_msg(char* msg, int logtype)
{
    switch(logtype)
    {
        case MSGHIST:
            fprintf(msghist_file, "%s\n", msg); 
            fflush(msghist_file);
            break;
        case DEBUG:
            fprintf(debug_file, "%s\n", msg); 
            fflush(debug_file);
            break;
    }
}

void log_format_msg(char* format, int logtype, ...)
{
    va_list args;
    va_start(args, logtype);

    char msg[256];
    vsnprintf(msg, 256, format, args);
    log_msg(msg, logtype);

    va_end(args);
}

void destroy_logs(void)
{
    fclose(msghist_file);
    fclose(debug_file);
}
