#include "console_command.h"

#include "variant.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PARAMS_MAX 8

int g_console_commands_count = 0;
struct ConsoleCommand* g_console_commands = NULL;

struct CommandParams
{
    int            count;
    struct Variant data[PARAMS_MAX];
};

struct Param
{
    char          name[32];
    enum DataType type;
    void*         conversion_func;
};

struct ConsoleCommand
{
    char                        name[32];
    int                         params_count;
    struct Param                params[PARAMS_MAX];
    ConsoleCommandExecutionFunc exec_func;
};

void console_command_init(struct ConsoleCommand* command, const char* name, ConsoleCommandExecutionFunc exec_func)
{
    memset(command, '\0', sizeof(struct ConsoleCommand));
    snprintf(command->name, sizeof(command->name), "%s", name);
    command->params_count = 0;
    command->exec_func = exec_func;
}

static void _console_command_add_param_internal(struct ConsoleCommand* command, const char* name, enum DataType type, void* func)
{
    assert(command->params_count < PARAMS_MAX && "Trying to add too many param types to a Console Command");
    snprintf(command->params[command->params_count].name, sizeof(command->params[command->params_count].name), "%s", name);
    command->params[command->params_count].type = type;
    command->params[command->params_count].conversion_func = func;
    ++command->params_count;
}

void console_command_add_param_vptr(struct ConsoleCommand* command, const char* name, ConsoleCommandParamsConversionFunc_vptr func)
{
    _console_command_add_param_internal(command, name, DATA_TYPE_PTR, func);
}

void console_command_add_param_int(struct ConsoleCommand* command, const char* name, ConsoleCommandParamsConversionFunc_int func)
{
    _console_command_add_param_internal(command, name, DATA_TYPE_INT, func);
}

void console_command_add_param_uint(struct ConsoleCommand* command, const char* name, ConsoleCommandParamsConversionFunc_uint func)
{
    _console_command_add_param_internal(command, name, DATA_TYPE_UINT, func);
}

void console_command_add_param_char(struct ConsoleCommand* command, const char* name, ConsoleCommandParamsConversionFunc_char func)
{
    _console_command_add_param_internal(command, name, DATA_TYPE_CHAR, func);
}

void console_command_add_param_string(struct ConsoleCommand* command, const char* name, ConsoleCommandParamsConversionFunc_string func)
{
    _console_command_add_param_internal(command, name, DATA_TYPE_STRING, func);
}

void console_command_execute(const struct ConsoleCommand* command, const struct CommandParams* params)
{
    command->exec_func(params);
}

const char* console_command_get_name(const struct ConsoleCommand* command)
{
    return command->name;
}

int console_command_get_param_count(const struct ConsoleCommand* command)
{
    return command->params_count;
}

const struct Param* console_command_get_param(const struct ConsoleCommand* command, int index)
{
    return &command->params[index];
}

struct CommandParams* command_params_new(void)
{
    struct CommandParams* params = malloc(sizeof(struct CommandParams));
    memset(params, 0, sizeof(struct CommandParams));
    return params;
}

void command_params_free(struct CommandParams* params)
{
    for(int i = 0; i < params->count; ++i)
    {
        if(params->data[i].type == DATA_TYPE_STRING)
        {
            free(params->data[i].data.as_str);
        }
    }
    free(params);
}

void command_params_add_param(struct CommandParams* params, struct Variant* param)
{ 
    assert(params->count < 8 && "Too many parameters added to console command!");
    params->data[params->count] = *param;
    ++params->count;
}

const struct Variant* command_params_get_param(const struct CommandParams* params, int index)
{
    return &params->data[index];
}

const char* param_get_name(const struct Param* param)
{
    return param->name;
}

enum DataType param_get_type(const struct Param* param)
{
    return param->type;
}

void* param_get_conversion_func(const struct Param* param)
{
    return param->conversion_func;
}

int convert_to_int(const char* param)
{
    return strtol(param, NULL, 10);
}

unsigned int convert_to_uint(const char* param)
{
    return strtoul(param, NULL, 10);
}

struct ConsoleCommand* console_command_look_up_by_name(const char* name)
{
    for(int i = 0; i < g_console_commands_count; ++i)
    {
        if(strcmp(name, g_console_commands[i].name) == 0)
        {
            return &g_console_commands[i];
        }
    }

    return NULL;
}

void init_console_command_array(int count)
{
    g_console_commands = malloc(sizeof(struct ConsoleCommand) * count);
    g_console_commands_count = count;
}

void uninit_console_command_array(void)
{
    free(g_console_commands);
    g_console_commands_count = 0;
}
