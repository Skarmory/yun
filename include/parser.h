#ifndef NAXX_PARSER_H
#define NAXX_PARSER_H

#include "list.h"

#include <stdbool.h>

enum ParserCode
{
    PARSER_OK,
    PARSER_NO_OP,
    PARSER_MALFORMED_FORMAT,
    PARSER_NO_FORMAT_FOUND,
    PARSER_MALFORMED_KEY_VALUE_PAIR,
    PARSER_MALFORMED_DATA,
    PARSER_UNKNOWN_DATA_TYPE,
    PARSER_FILE_OPEN_FAIL,
    PARSER_PARSE_CALLBACK_ERROR
};

enum ParseCallbackCode
{
    PARSE_CALLBACK_OK,
    PARSE_CALLBACK_ERROR
};

struct ParseState
{
    const char* filename;
    int line_no;
    char line[256];
    bool active;
};

struct Parser;

typedef enum ParseCallbackCode(*parse_method)(struct Parser*);

bool open_file_and_parse_all(struct Parser* parser, const char* filename);

struct Parser* parser_new(void);
void parser_free(struct Parser* parser);
void parser_get_state(struct Parser* parser, struct ParseState* state);
enum ParserCode parser_register_field(struct Parser* parser, char* field_name, char* field_data_format, parse_method pmeth);
enum ParserCode parser_parse(struct Parser* parser, char* line);
void parser_set_userdata(struct Parser* parser, void* userdata);
List* parser_get_userdata(struct Parser* parser);
void* parser_get_userdata_active(struct Parser* parser);
enum ParserCode parser_get_last_code(struct Parser* parser);

int parser_field_get_int(struct Parser* parser, const char* field_name, const char* field_data_name);
char parser_field_get_char(struct Parser* parser, const char* field_name, const char* field_data_name);
char* parser_field_get_string(struct Parser* parser, const char* field_name, const char* field_data_name);

void parser_print_formats(struct Parser* parser);

#endif
