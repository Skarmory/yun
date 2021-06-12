#include "parsing.h"

#include "log.h"
#include "spell.h"
#include "spell_effect.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* c_spells_file_name = "data/spells.txt";

parsing_callback(_parse_spells_finalise);
parsing_callback(_parse_spell_id_callback);
parsing_callback(_parse_spell_spatial_type_callback);
parsing_callback(_parse_spell_spell_effect_callback);

enum ParserCode parse_spells(void)
{
    struct Parser* parser = parser_new();

    parser_register_field(parser, "id", "id string", &_parse_spell_id_callback);
    parser_register_field(parser, "spatial-type", "spatial int", &_parse_spell_spatial_type_callback);
    parser_register_field(parser, "spell-effect", "id string", &_parse_spell_spell_effect_callback);

    if(open_file_and_parse_all(parser, c_spells_file_name))
    {
        _parse_spells_finalise(parser);
    }

    enum ParserCode retval = parser_get_last_code(parser);
    parser_free(parser);
    return retval;
}

parsing_callback(_parse_spells_finalise)
{
    struct List* spells_data = parser_get_userdata(parser);
    struct ListNode* node = NULL;

    log_format_msg(LOG_DEBUG, "Parsed spells count: %d", spells_data->count);

    g_spells = malloc(sizeof(struct Spell) * spells_data->count);
    g_spells_count = spells_data->count;

    int idx = 0;
    list_for_each(spells_data, node)
    {
        memcpy(&g_spells[idx++], node->data, sizeof(struct Spell));
        free(node->data);
    }

    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_spell_id_callback)
{
    struct Spell* spell = malloc(sizeof(struct Spell));
    memset(spell, 0, sizeof(struct Spell));
    list_init(&spell->spell_effects);

    parser_set_userdata(parser, spell);
    snprintf(spell->id, sizeof(spell->id), "%s", parser_field_get_string(parser, "id", "id"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_spell_spatial_type_callback)
{
    struct Spell* spell = parser_get_userdata_active(parser);
    spell->spatial_type = (enum SpellSpatialType)parser_field_get_int(parser, "spatial-type", "spatial");
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_spell_spell_effect_callback)
{
    struct Spell* spell = parser_get_userdata_active(parser);
    const char* spell_effect_id = parser_field_get_string(parser, "spell-effect", "id");

    list_add(&spell->spell_effects, spell_effect_look_up_by_id(spell_effect_id)); 

    return PARSE_CALLBACK_OK;
}
