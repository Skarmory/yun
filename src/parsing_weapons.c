#include "parsing.h"
#include "log.h"
#include "obj_weapon.h"
#include "mon_equip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* c_weapons_file_name = "data/weapons.txt";

static void _parse_weapons_finalise(struct Parser* parser);

static void _parse_weapon_name_callback(struct Parser* parser);
static void _parse_weapon_desc_callback(struct Parser* parser);
static void _parse_weapon_class_callback(struct Parser* parser);
static void _parse_weapon_damage_callback(struct Parser* parser);
static void _parse_weapon_attack_method_callback(struct Parser* parser);

enum ParserCode parse_weapons(void)
{
    struct Parser* parser = parser_new();

    parser_register_field(parser, "name", "name string", &_parse_weapon_name_callback);
    parser_register_field(parser, "desc", "desc string", &_parse_weapon_desc_callback);
    parser_register_field(parser, "class", "class string", &_parse_weapon_class_callback);
    parser_register_field(parser, "damage", "dice int sides int", &_parse_weapon_damage_callback);
    parser_register_field(parser, "attack-method", "name string", &_parse_weapon_attack_method_callback);

    bool result = open_file_and_parse_all(parser, c_weapons_file_name);
    if(!result)
    {
        struct ParseState state;
        parser_get_state(parser, &state);

        log_format_msg(DEBUG, "PARSE ERROR: %d", parser_get_last_code(parser));
        log_format_msg(DEBUG, "Parser state -- Active: %s | %d: %s", state.active ? "true" : "false", state.line_no, state.line);

        goto parse_end;
    }

    _parse_weapons_finalise(parser);

parse_end:;
    enum ParserCode retval = parser_get_last_code(parser);
    parser_free(parser);
    return retval;
}

static void _parse_weapons_finalise(struct Parser* parser)
{
    List* weapon_base_data = parser_get_userdata(parser);
    ListNode* node;

    log_format_msg(DEBUG, "Parsed weapon count: %d", weapon_base_data->count);

    g_weapon_base = malloc(sizeof(struct WeaponBase) * weapon_base_data->count);
    g_weapon_base_count = weapon_base_data->count;

    int idx = 0;
    list_for_each(weapon_base_data, node)
    {
        memcpy(&g_weapon_base[idx++], node->data, sizeof(struct WeaponBase));
        free(node->data);
    }
}

static void _parse_weapon_name_callback(struct Parser* parser)
{
    struct WeaponBase* base = malloc(sizeof(struct WeaponBase));
    memset(base, 0, sizeof(struct WeaponBase));

    parser_set_userdata(parser, base);

    snprintf(base->name, sizeof(base->name), "%s", parser_field_get_string(parser, "name", "name"));
}

static void _parse_weapon_desc_callback(struct Parser* parser)
{
    struct WeaponBase* base = parser_get_userdata_active(parser);

    snprintf(base->desc, sizeof(base->desc), "%s", parser_field_get_string(parser, "desc", "desc"));
}

static void _parse_weapon_class_callback(struct Parser* parser)
{
    struct WeaponBase* base = parser_get_userdata_active(parser);

    base->class = weapon_class_from_string(parser_field_get_string(parser, "class", "class"));
}

static void _parse_weapon_damage_callback(struct Parser* parser)
{
    struct WeaponBase* base = parser_get_userdata_active(parser);

    base->attk[0].num_dice = parser_field_get_int(parser, "damage", "dice");
    base->attk[0].sides_per_die = parser_field_get_int(parser, "damage", "sides");
}

static void _parse_weapon_attack_method_callback(struct Parser* parser)
{
    struct WeaponBase* base = parser_get_userdata_active(parser);

    base->attk[0].type = attack_method_lookup_by_name(parser_field_get_string(parser, "attack-method", "name"));
}
