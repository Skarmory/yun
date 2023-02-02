#include "console.h"

#include "console_command.h"
#include "console_input.h"
#include "message.h"
#include "ui.h"

#include <scieppend/core/log.h>
#include <scieppend/core/variant.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define INPUT_BUFFER_LENGTH MSGBOX_W

static void _parse_and_execute(char* input)
{
    // Get console command name first
    char* tok = strtok(input, " ");
    if(tok == NULL)
    {
        return;
    }

    const struct ConsoleCommand* cmd = console_command_look_up_by_name(tok);
    if(!cmd)
    {
        // No console command found
        return;
    }

    const int params_count = console_command_get_param_count(cmd);
    struct CommandParams* params = command_params_new();

    for(int i = 0; i < params_count; ++i)
    {
        tok = strtok(NULL, " ");
        if(!tok)
        {
            // Too few args
            log_format_msg(LOG_DEBUG, "ConsoleCommand: Too few arguments specified. Expected %d. Found: %d.", params_count, i+1);
            command_params_free(params);
            return;
        }

        struct Variant actual;
        const struct Param* param = console_command_get_param(cmd, i);
        void* cfunc = param_get_conversion_func(param);

        actual.type = param_get_type(param);
        switch(actual.type)
        {
            case DATA_TYPE_INT:
                actual.data.as_int = ((ConsoleCommandParamsConversionFunc_int)cfunc)(tok);
                break;
            case DATA_TYPE_UINT:
                actual.data.as_uint = ((ConsoleCommandParamsConversionFunc_uint)cfunc)(tok);
                break;
            case DATA_TYPE_PTR:
                actual.data.as_ptr = ((ConsoleCommandParamsConversionFunc_vptr)cfunc)(tok);
                break;
            case DATA_TYPE_STRING:
                {
                    int alloc_len = strlen(tok) + 1; // +1 for null terminator
                    actual.data.as_str = malloc(alloc_len);

                    if(cfunc)
                    {
                        snprintf(actual.data.as_str, alloc_len, "%s", ((ConsoleCommandParamsConversionFunc_string)cfunc)(tok));
                    }
                    else
                    {
                        snprintf(actual.data.as_str, alloc_len, "%s", tok);
                    }
                }
                break;
            case DATA_TYPE_CHAR:
                actual.data.as_char = ((ConsoleCommandParamsConversionFunc_char)cfunc)(tok);
                break;
        }

        command_params_add_param(params, &actual);
    }

    tok = strtok(NULL, " "); // Should make tok null if the correct number of args

    if(tok != NULL)
    {
        // Too many args
        int count = params_count + 1;
        while(tok != NULL)
        {
            tok = strtok(NULL, " ");
            ++count;
        }

        log_format_msg(LOG_DEBUG, "ConsoleCommand: Too many arguments specified. Expected %d. Found: %d.", params_count, count);
        command_params_free(params);
        return;
    }

    console_command_execute(cmd, params);
    command_params_free(params);
}

void console(void)
{
    char in_command[INPUT_BUFFER_LENGTH];

    do
    {
        memset(in_command, '\0', INPUT_BUFFER_LENGTH);

        if(console_input(in_command))
        {
            return;
        }

        _parse_and_execute(in_command);
        display_main_screen();
    }
    while(true);
}
