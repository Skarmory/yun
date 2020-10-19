#include "parser.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define METADATA_FIELD_NAME_SIZE 32
#define FIELD_NAME_SIZE 32
#define FIELD_DATA_SIZE 256

const char c_key_value_separator = ':';
const char c_value_separator = ';';
const char c_comment = '#';
const char c_new_line = '\n';

const char* c_tok_int = "int";
const char* c_tok_char = "char";
const char* c_tok_string = "string";
const char* c_tok_bool = "bool";

typedef List ParseFormat_List;
typedef List FieldData_List;
typedef List Metadata_List;
typedef List Userdata_List;

enum DataType
{
    TYPE_INT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_ERROR
};

union DataValue
{
    int   as_int;
    char  as_char;
    char* as_str;
    bool  as_bool;
};

struct Metadata
{
    char          name[METADATA_FIELD_NAME_SIZE];
    enum DataType type;
};

// A piece of data and a pointer to its metadata type
struct FieldData
{
    struct Metadata* meta;
    union DataValue value;
};

// Collection of field data for a single entry
struct ParseData
{
    char name[FIELD_NAME_SIZE];
    FieldData_List data;
};

// Collection of metadata for each field data in an entry and a parse function callback
struct ParseFormat
{
    char field_name[FIELD_NAME_SIZE];
    Metadata_List field_meta;
    parse_method method;
};

struct Parser
{
    enum ParserCode   last_code;
    Userdata_List     userdata;
    ParseFormat_List  parse_formats;
    struct ParseData  parse_data;
    struct ParseState parse_state;
};

/* ----- FDECL ----- */

static inline struct FieldData* _field_data_new(struct Metadata* meta);
static inline void _field_data_free(struct FieldData* field);
static inline struct Metadata* _metadata_new(void);
static inline void _metadata_free(struct Metadata* meta);
static void _parse_format_free(struct ParseFormat* format);
static enum DataType _to_data_type(char* str);
static enum ParserCode _split_key_value(char* field_name, char* field_data, char* line);
static enum ParserCode _parse_field_data(struct ParseData* parse_data, struct ParseFormat* parse_format, char* field_data);

static inline void _parser_reset(struct Parser* parser);
static void _parser_free_parse_formats(struct Parser* parser);
static void _parser_free_userdata(struct Parser* parser);
static enum ParserCode _parser_find_format(struct Parser* parser, const char* field_name, struct ParseFormat** format);
struct FieldData* _field_get_value(struct Parser* parser, const char* field_name, const char* field_data_name);

void _print_metadata(struct Metadata* meta);
void _print_format(struct ParseFormat* format);
void _print_parse_data(struct Parser* parser);

/* ----- PARSING HELPER METHODS ----- */

bool open_file_and_parse_all(struct Parser* parser, const char* filename)
{
    parser->parse_state.filename = filename;

    FILE* file = fopen(filename, "r");

    if(!file)
    {
        parser->last_code = PARSER_FILE_OPEN_FAIL;
        return false;
    }

    char* line = NULL;
    size_t sz = 0;

    while(getline(&line, &sz, file) != -1)
    {
        parser->last_code = parser_parse(parser, line);
        if(parser->last_code != PARSER_OK && parser->last_code != PARSER_NO_OP)
        {
            if(parser->last_code == PARSER_NO_FORMAT_FOUND)
            {
                log_msg(DEBUG, "Parser error: no format found");
            }
            else if(parser->last_code == PARSER_MALFORMED_DATA)
            {
                log_msg(DEBUG, "Parser error: malformed data");
                break;
            }
            else if(parser->last_code == PARSER_MALFORMED_KEY_VALUE_PAIR)
            {
                log_msg(DEBUG, "Parser error: malformed key pair value");
                break;
            }
            else if(parser->last_code == PARSER_UNKNOWN_DATA_TYPE)
            {
                log_msg(DEBUG, "Parser error: unknown data type");
                break;
            }
            else if(parser->last_code == PARSER_PARSE_CALLBACK_ERROR)
            {
                log_msg(DEBUG, "Parser error: parse callback error");
                break;
            }
        }
    }

    free(line);
    fclose(file);

    bool success = (parser->last_code == PARSER_OK || parser->last_code == PARSER_NO_OP);
    if(!success)
    {
        log_format_msg(DEBUG, "PARSE ERROR: %d", parser_get_last_code(parser));
        log_format_msg(DEBUG, "Parser state -- %s | Active: %s | %d: %s", parser->parse_state.filename, parser->parse_state.active ? "true" : "false", parser->parse_state.line_no, parser->parse_state.line);
    }

    return (parser->last_code == PARSER_OK || parser->last_code == PARSER_NO_OP);
}

static inline struct FieldData* _field_data_new(struct Metadata* meta)
{
    struct FieldData* data = malloc(sizeof(struct FieldData));
    data->meta = meta;
    return data;
}

static inline void _field_data_free(struct FieldData* field)
{
    if(field->meta->type == TYPE_STRING)
        free(field->value.as_str);

    free(field);
}

static inline struct Metadata* _metadata_new(void)
{
    struct Metadata* meta = malloc(sizeof(struct Metadata));
    return meta;
}

static inline void _metadata_free(struct Metadata* meta)
{
    free(meta);
}

static void _parse_format_free(struct ParseFormat* format)
{
    ListNode *metadata_node, *metadata_node_next;
    list_for_each_safe(&format->field_meta, metadata_node, metadata_node_next)
    {
        _metadata_free(metadata_node->data);
        free(metadata_node);
    }

    free(format);
}

static enum DataType _to_data_type(char* str)
{
    if(strcmp(str, c_tok_int) == 0)
        return TYPE_INT;

    if(strcmp(str, c_tok_char) == 0)
        return TYPE_CHAR;

    if(strcmp(str, c_tok_string) == 0)
        return TYPE_STRING;

    if(strcmp(str, c_tok_bool) == 0)
    {
        return TYPE_BOOL;
    }

    return TYPE_ERROR;
}

static enum ParserCode _split_key_value(char* field_name, char* field_data, char* line)
{
    char* tok = NULL;

    if(!(tok = strtok(line, ":")))
    {
        return PARSER_MALFORMED_KEY_VALUE_PAIR;
    }

    snprintf(field_name, 32, "%s", tok);

    if(!(tok = strtok(NULL, ":")))
    {
        return PARSER_MALFORMED_KEY_VALUE_PAIR;
    }

    int len = strlen(tok);
    if(tok[len-1] == '\n')
        tok[len-1] = '\0';
    snprintf(field_data, 256, "%s", tok);

    return PARSER_OK;
}

static enum ParserCode _parse_field_data(struct ParseData* parse_data, struct ParseFormat* parse_format, char* field_data)
{
    char* tok = strtok(field_data, ";");

    ListNode* node;
    list_for_each(&parse_format->field_meta, node)
    {
        if(!tok)
        {
            return PARSER_MALFORMED_DATA;
        }

        union DataValue value;
        struct Metadata* meta = node->data;

        switch(meta->type)
        {
            case TYPE_INT:
            {
                value.as_int = strtol(tok, NULL, 0);
                break;
            }
            case TYPE_CHAR:
            {
                value.as_char = tok[0];
                break;
            }
            case TYPE_STRING:
            {
                int len = strlen(tok) + 1;
                value.as_str = malloc(len); 
                snprintf(value.as_str, len, "%s", tok); 
                break;
            }
            case TYPE_BOOL:
            {
                value.as_bool = (strcmp(tok, "true") == 0) ? true : false;
                break;
            }
            case TYPE_ERROR:
            {
                return PARSER_UNKNOWN_DATA_TYPE;
            }
        }

        struct FieldData* field_data = _field_data_new(node->data);
        field_data->value = value;
        list_add(&parse_data->data, field_data);

        tok = strtok(NULL, ";");
    }

    snprintf(parse_data->name, sizeof(parse_data->name), "%s", parse_format->field_name);

    return PARSER_OK;
}

/* ----- PARSER METHODS ----- */

static inline void _parser_reset(struct Parser* parser)
{
    if(parser->parse_data.data.count > 0)
    {
        ListNode *field_node, *field_node_next;
        list_for_each_safe(&parser->parse_data.data, field_node, field_node_next)
        {
            _field_data_free(field_node->data);
            list_rm(&parser->parse_data.data, field_node);
        }
    }
}

static void _parser_free_parse_formats(struct Parser* parser)
{
    ListNode* node, *next;
    list_for_each_safe(&parser->parse_formats, node, next)
    {
        _parse_format_free(node->data);
        free(node);
    }
}

static void _parser_free_userdata(struct Parser* parser)
{
    ListNode* node, *next;
    list_for_each_safe(&parser->userdata, node, next)
    {
        free(node);
    }
}

// Find a format with a name that corresponds to the current entry name
static enum ParserCode _parser_find_format(struct Parser* parser, const char* field_name, struct ParseFormat** format)
{
    enum ParserCode code = PARSER_NO_FORMAT_FOUND;
    ListNode* n;
    list_for_each(&parser->parse_formats, n)
    {
        *format = (struct ParseFormat*)n->data;
        if(strcmp(field_name, (*format)->field_name) == 0)
        {
            code = PARSER_OK;
            break;
        }
    }

    if(code == PARSER_NO_FORMAT_FOUND)
    {
        *format = NULL;
    }

    return code;
}

struct FieldData* _field_get_value(struct Parser* parser, const char* field_name, const char* field_data_name)
{
    struct ParseData* pdata = &parser->parse_data;
    if(strcmp(pdata->name, field_name) == 0)
    {
        ListNode* n;
        list_for_each(&pdata->data, n)
        {
            struct FieldData* fdata = n->data;
            if(strcmp(fdata->meta->name, field_data_name) == 0)
            {
                return fdata;
            }
        }
    }

    return NULL;
}

struct Parser* parser_new(void)
{
    struct Parser* parser = malloc(sizeof(struct Parser)); 
    parser->parse_state.line_no = -1;
    memset(parser->parse_state.line, '\0', sizeof(parser->parse_state.line));
    parser->parse_state.active = false;
    list_init(&parser->userdata);
    list_init(&parser->parse_formats);
    list_init(&parser->parse_data.data);

    return parser;
}

void parser_free(struct Parser* parser)
{
    _parser_reset(parser);
    _parser_free_parse_formats(parser);
    _parser_free_userdata(parser);

    free(parser);
}

void parser_get_state(struct Parser* parser, struct ParseState* state)
{
    state->filename = parser->parse_state.filename;
    state->line_no = parser->parse_state.line_no;
    snprintf(state->line, sizeof(state->line), "%s", parser->parse_state.line);
    state->active = parser->parse_state.active;
}

enum ParserCode parser_register_field(struct Parser* parser, char* field_name, char* field_data_format, parse_method method)
{
    enum ParserCode code = PARSER_OK;

    struct ParseFormat* format;
    if((code = _parser_find_format(parser, field_name, &format)) != PARSER_OK)
    {
        format = (struct ParseFormat*)malloc(sizeof(struct ParseFormat));
        list_add(&parser->parse_formats, format);
    }

    snprintf(format->field_name, 32, "%s", field_name);
    list_init(&format->field_meta);
    format->method = method;

    int fmt_len = strlen(field_data_format) + 1;
    char* fmt = (char*)malloc(fmt_len);
    snprintf(fmt, fmt_len, "%s", field_data_format);

    char* tok = strtok(fmt, " ");
    if(tok)
    {
        while(tok)
        {
            struct Metadata* meta = _metadata_new();
            list_add(&format->field_meta, meta);

            snprintf(meta->name, METADATA_FIELD_NAME_SIZE, "%s", tok);

            if(!(tok = strtok(NULL, " ")))
            {
                code = PARSER_MALFORMED_FORMAT;
                break;
            }

            meta->type = _to_data_type(tok);

            tok = strtok(NULL, " ");
        }
    }
    else
    {
        code = PARSER_MALFORMED_FORMAT;
    }

    free(fmt);

    if(code == PARSER_MALFORMED_FORMAT)
    {
        list_uninit(&format->field_meta);
        list_rm(&parser->parse_formats, list_find(&parser->parse_formats, format)); 
        free(format);
    }

    return code;
}

enum ParserCode parser_parse(struct Parser* parser, char* line)
{
    enum ParserCode code = PARSER_OK;

    parser->parse_state.line_no++;
    snprintf(parser->parse_state.line, sizeof(parser->parse_state.line), "%s", line);

    // Disregard comments
    if(line[0] == c_comment)
        return PARSER_NO_OP;

    // If the line is just a new line character
    // a) if it immediately procedes an entry, set parse state to inactive
    // b) if it is anywhere else, disregard this line
    if(line[0] == c_new_line)
    {
        if(parser->parse_state.active)
            parser->parse_state.active = false;
        return PARSER_NO_OP;
    }

    // This should be a proper line of data now
    // If the parsing state is not active, this is a new entry
    if(!parser->parse_state.active)
        parser->parse_state.active = true;

    _parser_reset(parser);

    // Split the field into field name and field data
    char field_name[FIELD_NAME_SIZE];
    char field_data[FIELD_DATA_SIZE];
    struct ParseFormat* format;

    if((code = _split_key_value(field_name, field_data, line)) != PARSER_OK)
        return code;

    if((code = _parser_find_format(parser, field_name, &format)) == PARSER_NO_FORMAT_FOUND)
        return code;

    if((code = _parse_field_data(&parser->parse_data, format, field_data)) != PARSER_OK)
        return code;

    enum ParseCallbackCode method_code = format->method(parser);
    if(method_code == PARSE_CALLBACK_ERROR)
        code = PARSER_PARSE_CALLBACK_ERROR;

    return code;
}

void parser_set_userdata(struct Parser* parser, void* userdata)
{
    list_add(&parser->userdata, userdata);
}

void* parser_get_userdata_active(struct Parser* parser)
{
    return parser->userdata.tail->data;
}

List* parser_get_userdata(struct Parser* parser)
{
    return &parser->userdata;
}

enum ParserCode parser_get_last_code(struct Parser* parser)
{
    return parser->last_code;
}

int parser_field_get_int(struct Parser* parser, const char* field_name, const char* field_data_name)
{
    struct FieldData* fdata = _field_get_value(parser, field_name, field_data_name);
    if(!fdata)
        return -1;

    return fdata->value.as_int;
}

char parser_field_get_char(struct Parser* parser, const char* field_name, const char* field_data_name)
{
    struct FieldData* fdata = _field_get_value(parser, field_name, field_data_name);
    if(!fdata)
        return '\0';

    return fdata->value.as_char;
}

char* parser_field_get_string(struct Parser* parser, const char* field_name, const char* field_data_name)
{
    struct FieldData* fdata = _field_get_value(parser, field_name, field_data_name);
    if(!fdata)
        return NULL;

    return fdata->value.as_str;
}

bool parser_field_get_bool(struct Parser* parser, const char* field_name, const char* field_data_name)
{
    struct FieldData* fdata = _field_get_value(parser, field_name, field_data_name);
    if(!fdata)
    {
        return false;
    }

    return fdata->value.as_bool;
}

/* ----- DEBUG METHODS ----- */

void _print_metadata(struct Metadata* meta)
{
    printf("metadata: %s, %d\n", meta->name, meta->type);
}

void _print_format(struct ParseFormat* f)
{
    printf("\tfield_name: %s\n", f->field_name);
    printf("\tfield_data:\n");

    ListNode* n2;
    list_for_each(&f->field_meta, n2)
    {
        printf("\t\t");
        _print_metadata(n2->data);
    }
}

void parser_print_formats(struct Parser* p)
{
    printf("FORMATS\n");
    int format_count = 1;
    ListNode* n = NULL;
    list_for_each(&p->parse_formats, n)
    {
        printf("format %d\n", format_count);
        _print_format(n->data);

        format_count++;
        printf("\n");
    }
}

void _print_parse_data(struct Parser* parser)
{
    printf("Parse data\n");
    ListNode* n;
    list_for_each(&parser->parse_data.data, n)
    {
        struct FieldData* fdata = n->data;
        printf("name: %s\n", fdata->meta->name);
        switch(fdata->meta->type)
        {
            case TYPE_INT:
                printf("type: INT\n");
                printf("data: %d\n", fdata->value.as_int);
                break;
            case TYPE_CHAR:
                printf("type: CHAR\n");
                printf("data: %c\n", fdata->value.as_char);
                break;
            case TYPE_STRING:
                printf("type: STRING\n");
                printf("data: %s\n", fdata->value.as_str);
                break;
            case TYPE_BOOL:
                printf("type: BOOL\n");
                printf("data: %d", fdata->value.as_bool);
                break;
            case TYPE_ERROR:
                printf("type: ERROR. Something went wrong here!\n");
                break;
        }
    }
}
