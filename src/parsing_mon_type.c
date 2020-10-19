#include "parsing.h"
#include "colour.h"
#include "obj_armour.h"
#include "obj_weapon.h"
#include "log.h"
#include "mon_type.h"
#include "symbol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* c_mon_type_file_name = "data/monster_types.txt";

parsing_callback(_parse_mon_types_finalise);
parsing_callback(_parse_mon_type_id_callback);
parsing_callback(_parse_mon_type_name_callback);
parsing_callback(_parse_mon_type_desc_callback);
parsing_callback(_parse_mon_type_symbol_callback);
parsing_callback(_parse_mon_type_colour_callback);
parsing_callback(_parse_mon_type_stats_callback);
parsing_callback(_parse_mon_type_pathing_callback);
parsing_callback(_parse_mon_type_base_armour_callback);
parsing_callback(_parse_mon_type_base_weapon_callback);
parsing_callback(_parse_mon_type_vision_radius_callback);

enum ParserCode parse_mon_types(void)
{
    struct Parser* parser = parser_new();

    parser_register_field(parser, "id", "id string", &_parse_mon_type_id_callback);
    parser_register_field(parser, "name", "name string", &_parse_mon_type_name_callback);
    parser_register_field(parser, "desc", "desc string", &_parse_mon_type_desc_callback);
    parser_register_field(parser, "symbol", "symbol char", &_parse_mon_type_symbol_callback);
    parser_register_field(parser, "colour", "red int green int blue int", &_parse_mon_type_colour_callback);
    parser_register_field(parser, "stats", "strength int agility int intelligence int spirit int stamina int", &_parse_mon_type_stats_callback);
    parser_register_field(parser, "move-flag", "flag string", &_parse_mon_type_pathing_callback);
    parser_register_field(parser, "base-armour", "id string", &_parse_mon_type_base_armour_callback);
    parser_register_field(parser, "base-weapon", "name string", &_parse_mon_type_base_weapon_callback);
    parser_register_field(parser, "vision-radius", "radius int", &_parse_mon_type_vision_radius_callback);

    if(open_file_and_parse_all(parser, c_mon_type_file_name))
    {
        _parse_mon_types_finalise(parser);
    }

    enum ParserCode retval = parser_get_last_code(parser);
    parser_free(parser);
    return retval;
}

parsing_callback(_parse_mon_types_finalise)
{
    List* mon_type_data = parser_get_userdata(parser);
    ListNode* node;

    log_format_msg(DEBUG, "Parsed mon type count: %d", mon_type_data->count);

    g_mon_type = malloc(sizeof(struct MonType) * mon_type_data->count);
    g_mon_type_count = mon_type_data->count;

    int idx = 0;
    list_for_each(mon_type_data, node)
    {
        memcpy(&g_mon_type[idx++], node->data, sizeof(struct MonType));
        free(node->data);
    }

    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_mon_type_id_callback)
{
    struct MonType* type = malloc(sizeof(struct MonType));
    memset(type, 0, sizeof(struct MonType));

    parser_set_userdata(parser, type);
    snprintf(type->id, sizeof(type->id), "%s", parser_field_get_string(parser, "id", "id"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_mon_type_name_callback)
{
    struct MonType* type = parser_get_userdata_active(parser);
    snprintf(type->name, sizeof(type->name), "%s", parser_field_get_string(parser, "name", "name"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_mon_type_desc_callback)
{
    struct MonType* type = parser_get_userdata_active(parser);
    snprintf(type->desc, sizeof(type->desc), "%s", parser_field_get_string(parser, "desc", "desc"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_mon_type_symbol_callback)
{
    struct MonType* type = parser_get_userdata_active(parser);
    type->symbol = malloc(sizeof(struct Symbol));
    memset(type->symbol, 0, sizeof(struct Symbol));
    
    type->symbol->sym = parser_field_get_char(parser, "symbol", "symbol");
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_mon_type_colour_callback)
{
    struct MonType* type = parser_get_userdata_active(parser);
    type->symbol->fg.r = parser_field_get_int(parser, "colour", "red");
    type->symbol->fg.g = parser_field_get_int(parser, "colour", "green");
    type->symbol->fg.b = parser_field_get_int(parser, "colour", "blue");
    type->symbol->bg = *COL(CLR_DEFAULT);
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_mon_type_stats_callback)
{
    struct MonType* type = parser_get_userdata_active(parser);
    type->strength     = parser_field_get_int(parser, "stats", "strength");
    type->agility      = parser_field_get_int(parser, "stats", "agility");
    type->intelligence = parser_field_get_int(parser, "stats", "intelligence");
    type->spirit       = parser_field_get_int(parser, "stats", "spirit");
    type->stamina      = parser_field_get_int(parser, "stats", "stamina");
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_mon_type_pathing_callback)
{
    struct MonType* type = parser_get_userdata_active(parser);
    type->move_flags |= mon_attr_move_flag_from_string(parser_field_get_string(parser, "move-flag", "flag"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_mon_type_base_armour_callback)
{
    struct MonType* type = parser_get_userdata_active(parser);
    type->base_armour = armour_base_look_up_by_id(parser_field_get_string(parser, "base-armour", "id"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_mon_type_base_weapon_callback)
{
    struct MonType* type = parser_get_userdata_active(parser);
    type->base_weapon = weapon_base_lookup_by_name(parser_field_get_string(parser, "base-weapon", "name"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_mon_type_vision_radius_callback)
{
    struct MonType* type = parser_get_userdata_active(parser);
    type->vision_radius = parser_field_get_int(parser, "vision-radius", "radius");
    return PARSE_CALLBACK_OK;
}
