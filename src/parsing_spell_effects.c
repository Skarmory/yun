#include "parsing.h"

#include "log.h"
#include "spell_effect.h"
#include "spell_effect_action.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* c_spell_effects_file_name = "data/spell_effects.txt";

parsing_callback(_parse_spell_effects_finalise);
parsing_callback(_parse_spell_effect_id_callback);
parsing_callback(_parse_spell_effect_event_type_callback);
parsing_callback(_parse_spell_effect_action_type_callback);
parsing_callback(_parse_spell_effect_cause_damage_callback);

enum ParserCode parse_spell_effects(void)
{
    struct Parser* parser = parser_new();

    parser_register_field(parser, "id", "id string", &_parse_spell_effect_id_callback);
    parser_register_field(parser, "event-type", "event int", &_parse_spell_effect_event_type_callback);
    parser_register_field(parser, "action-type", "action int", &_parse_spell_effect_action_type_callback);
    parser_register_field(parser, "damage", "dice-count int dice-sides int", &_parse_spell_effect_cause_damage_callback);

    if(open_file_and_parse_all(parser, c_spell_effects_file_name))
    {
        _parse_spell_effects_finalise(parser);
    }

    enum ParserCode retval = parser_get_last_code(parser);
    parser_free(parser);
    return retval;
}

parsing_callback(_parse_spell_effects_finalise)
{
    List* spell_effects_data = parser_get_userdata(parser);
    ListNode* node = NULL;

    log_format_msg(LOG_DEBUG, "Parsed spell effects count: %d", spell_effects_data->count);

    g_spell_effects = malloc(sizeof(struct SpellEffect) * spell_effects_data->count);
    g_spell_effects_count = spell_effects_data->count;

    int idx = 0;
    list_for_each(spell_effects_data, node)
    {
        memcpy(&g_spell_effects[idx++], node->data, sizeof(struct SpellEffect));
        free(node->data);
    }

    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_spell_effect_id_callback)
{
    struct SpellEffect* spell_effect = malloc(sizeof(struct SpellEffect));
    memset(spell_effect, 0, sizeof(struct SpellEffect));

    parser_set_userdata(parser, spell_effect);
    snprintf(spell_effect->id, sizeof(spell_effect->id), "%s", parser_field_get_string(parser, "id", "id"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_spell_effect_event_type_callback)
{
    struct SpellEffect* spell_effect = parser_get_userdata_active(parser);
    spell_effect->event = (enum SpellEffectEvent)parser_field_get_int(parser, "event-type", "event");
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_spell_effect_action_type_callback)
{
    struct SpellEffect* spell_effect = parser_get_userdata_active(parser);
    spell_effect->action = (enum SpellEffectAction)parser_field_get_int(parser, "action-type", "action");
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_spell_effect_cause_damage_callback)
{
    struct SpellEffect* spell_effect = parser_get_userdata_active(parser);

    spell_effect->action_data = malloc(sizeof(struct SpellEffectAction_CauseDamage));
    ((struct SpellEffectAction_CauseDamage*)spell_effect->action_data)->dice_count = parser_field_get_int(parser, "damage", "dice-count");
    ((struct SpellEffectAction_CauseDamage*)spell_effect->action_data)->dice_sides = parser_field_get_int(parser, "damage", "dice-sides");

    return PARSE_CALLBACK_OK;
}
