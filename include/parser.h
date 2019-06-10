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
    PARSER_UNKNOWN_DATA_TYPE
};

struct ParseState
{
    int line_no;
    const char* line;
    bool active;
};

struct Parser;

typedef void(*parse_method)(struct Parser*);

struct Parser* parser_new(void);
void parser_free(struct Parser* parser);
void parser_get_state(struct Parser* parser, struct ParseState* state);
enum ParserCode parser_register_field(struct Parser* parser, char* field_name, char* field_data_format, parse_method pmeth);
enum ParserCode parser_parse(struct Parser* parser, char* line);
void parser_set_userdata(struct Parser* parser, void* userdata);
List* parser_get_userdata(struct Parser* parser);
void* parser_get_userdata_active(struct Parser* parser);

int parser_field_get_int(struct Parser* parser, const char* field_name, const char* field_data_name);
char parser_field_get_char(struct Parser* parser, const char* field_name, const char* field_data_name);
char* parser_field_get_string(struct Parser* parser, const char* field_name, const char* field_data_name);

void parser_print_formats(struct Parser* parser);

#endif
