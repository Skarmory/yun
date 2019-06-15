#include "parsing.h"
#include "log.h"
#include "obj_armour.h"
#include "mon_equip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* c_armours_file_name = "data/armours.txt";

static void _parse_armours_finalise(struct Parser* parser);

static void _parse_armour_name_callback(struct Parser* parser);
static void _parse_armour_desc_callback(struct Parser* parser);
static void _parse_armour_slot_callback(struct Parser* parser);
static void _parse_armour_class_callback(struct Parser* parser);
static void _parse_armour_armour_value_callback(struct Parser* parser);

enum ParserCode parse_armours(void)
{
    struct Parser* parser = parser_new();

    parser_register_field(parser, "name", "name string", &_parse_armour_name_callback);
    parser_register_field(parser, "desc", "desc string", &_parse_armour_desc_callback);
    parser_register_field(parser, "slot", "slot string", &_parse_armour_slot_callback);
    parser_register_field(parser, "class", "class string", &_parse_armour_class_callback);
    parser_register_field(parser, "armour-value", "value int", &_parse_armour_armour_value_callback);

    bool result = open_file_and_parse_all(parser, c_armours_file_name);
    if(!result)
    {
        struct ParseState state;
        parser_get_state(parser, &state);

        log_format_msg(DEBUG, "PARSE ERROR: %d", parser_get_last_code(parser));
        log_format_msg(DEBUG, "Parser state -- Active: %s | %d: %s", state.active ? "true" : "false", state.line_no, state.line);

        goto parse_end;
    }

    _parse_armours_finalise(parser);

parse_end:;
    enum ParserCode retval = parser_get_last_code(parser);
    parser_free(parser);
    return retval;
}

static void _parse_armours_finalise(struct Parser* parser)
{
    List* armour_base_data = parser_get_userdata(parser);
    ListNode* node;

    log_format_msg(DEBUG, "Parsed armour count: %d", armour_base_data->count);

    g_armour_base = malloc(sizeof(struct ArmourBase) * armour_base_data->count);
    g_armour_base_count = armour_base_data->count;

    int idx = 0;
    list_for_each(armour_base_data, node)
    {
        memcpy(&g_armour_base[idx++], node->data, sizeof(struct ArmourBase));
        free(node->data);
    }
}

static void _parse_armour_name_callback(struct Parser* parser)
{
    struct ArmourBase* base = malloc(sizeof(struct ArmourBase));
    memset(base, 0, sizeof(struct ArmourBase));

    parser_set_userdata(parser, base);

    snprintf(base->name, sizeof(base->name), "%s", parser_field_get_string(parser, "name", "name"));
}

static void _parse_armour_desc_callback(struct Parser* parser)
{
    struct ArmourBase* base = parser_get_userdata_active(parser);

    snprintf(base->desc, sizeof(base->desc), "%s", parser_field_get_string(parser, "desc", "desc"));
}

static void _parse_armour_slot_callback(struct Parser* parser)
{
    struct ArmourBase* base = parser_get_userdata_active(parser);

    base->slot = equipment_slot_by_name(parser_field_get_string(parser, "slot", "slot"));
}

static void _parse_armour_class_callback(struct Parser* parser)
{
    struct ArmourBase* base = parser_get_userdata_active(parser);

    base->class = armour_class_from_string(parser_field_get_string(parser, "class", "class"));
}

static void _parse_armour_armour_value_callback(struct Parser* parser)
{
    struct ArmourBase* base = parser_get_userdata_active(parser);

    base->armour_value = parser_field_get_int(parser, "armour-value", "value");
}
