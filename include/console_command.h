#ifndef YUN_CONSOLE_COMMAND_H
#define YUN_CONSOLE_COMMAND_H

#include <scieppend/core/variant.h>

struct ConsoleCommand;
struct CommandParams;
struct Param;
struct Variant;

typedef void         (*ConsoleCommandExecutionFunc)(const struct CommandParams* params);
typedef void*        (*ConsoleCommandParamsConversionFunc_vptr)(const char* param);
typedef int          (*ConsoleCommandParamsConversionFunc_int) (const char* param);
typedef unsigned int (*ConsoleCommandParamsConversionFunc_uint)(const char* param);
typedef char         (*ConsoleCommandParamsConversionFunc_char)(const char* param);
typedef const char*  (*ConsoleCommandParamsConversionFunc_string)(const char* param);

void                  console_command_init(struct ConsoleCommand* command, const char* name, ConsoleCommandExecutionFunc exec_func);
void                  console_command_add_param_vptr(struct ConsoleCommand* command, const char* name, ConsoleCommandParamsConversionFunc_vptr func);
void                  console_command_add_param_int(struct ConsoleCommand* command, const char* name, ConsoleCommandParamsConversionFunc_int func);
void                  console_command_add_param_uint(struct ConsoleCommand* command, const char* name, ConsoleCommandParamsConversionFunc_uint func);
void                  console_command_add_param_char(struct ConsoleCommand* command, const char* name, ConsoleCommandParamsConversionFunc_char func);
void                  console_command_add_param_string(struct ConsoleCommand* command, const char* name, ConsoleCommandParamsConversionFunc_string func);
void                  console_command_execute(const struct ConsoleCommand* command, const struct CommandParams* params);
const char*           console_command_get_name(const struct ConsoleCommand* command);
int                   console_command_get_param_count(const struct ConsoleCommand* command);
const struct Param*   console_command_get_param(const struct ConsoleCommand* command, int index);

struct CommandParams* command_params_new(void);
void                  command_params_free(struct CommandParams* params);
void                  command_params_add_param(struct CommandParams* params, struct Variant* param);
const struct Variant* command_params_get_param(const struct CommandParams* params, int index);

const char*           param_get_name(const struct Param* param);
enum DataType         param_get_type(const struct Param* param);
void*                 param_get_conversion_func(const struct Param* param);

// Primitive conversions
int          convert_to_int(const char* param);
unsigned int convert_to_uint(const char* param);

extern int g_console_commands_count;
extern struct ConsoleCommand* g_console_commands;

struct ConsoleCommand* console_command_look_up_by_name(const char* name);
void init_console_command_array(int count);
void uninit_console_command_array(void);

#endif
