#include "init.h"

#include "components/components_init.h"
#include "feature.h"
#include "gameplay.h"
#include "globals.h"
#include "console_commands_init.h"
#include "mon_attack.h"
#include "mon_type.h"
#include "obj_armour.h"
#include "obj_weapon.h"
#include "parsing.h"
#include "spell.h"
#include "spell_effect.h"
#include "symbols.h"
#include "systems/turn_system.h"
#include "systems/turn_take_system.h"
#include "terminal_renderer.h"
#include "util.h"

#include <scieppend/core/colour.h>
#include <scieppend/core/ecs.h>
#include <scieppend/core/log.h>
#include <scieppend/core/symbol.h>
#include <scieppend/core/tasking.h>
#include <scieppend/core/term.h>

#include <stddef.h>
#include <stdlib.h>
#include <time.h>

static inline bool _init_gamedata(void)
{
    log_msg(LOG_DEBUG, "parsing attack types");
    if(parse_attack_methods() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing armours");
    if(parse_armours() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing weapons");
    if(parse_weapons() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing mon types");
    if(parse_mon_types() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing features");
    if(parse_features() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing spell effects");
    if(parse_spell_effects() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing spells");
    if(parse_spells() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    log_msg(LOG_DEBUG, "parsing terrains");
    if(parse_terrains() != PARSER_OK)
    {
        log_msg(LOG_DEBUG, "parsing failed");
        return false;
    }
    log_msg(LOG_DEBUG, "parsing complete");

    return true;
}

static void _uninit_gamedata(void)
{
    for(int idx = 0; idx < g_spells_count; ++idx)
    {
        list_uninit(&g_spells[idx].spell_effects);
    }
    free(g_spells);
    g_spells_count = 0;

    for(int idx = 0; idx < g_spell_effects_count; ++idx)
    {
        free(g_spell_effects[idx].action_data);
    }
    free(g_spell_effects);
    g_spell_effects_count = 0;

    for(int idx = 0; idx < g_features_count; ++idx)
    {
        free(g_features[idx].symbol);
    }

    free(g_features);
    g_features_count = 0;

    for(int idx = 0; idx < g_mon_type_count; ++idx)
    {
        free(g_mon_type[idx].symbol);
    }

    free(g_mon_type);
    g_mon_type_count = 0;

    free(g_weapon_base);
    g_weapon_base_count = 0;

    free(g_armour_base);
    g_armour_base_count = 0;

    free(g_attack_methods);
    g_attack_methods_count = 0;
}

bool init_yun(void)
{
    srand(time(NULL));

    init_logs();

    eventing_init();
    ecs_init();
    term_init();
    term_set_sigint_callback(&sigint_handler);
    term_get_wh(&screen_cols, &screen_rows);

    g_tasker = tasker_new();

    symbols_init();

    if(!_init_gamedata())
    {
        return false;
    }

    init_console_commands();
    components_init();
    terminal_renderer_init();
    turn_system_init();
    turn_take_system_init();

    return true;
}

void uninit_yun(void)
{
    turn_take_system_uninit();
    turn_system_uninit();
    terminal_renderer_uninit();
    components_uninit();
    uninit_console_commands();
    _uninit_gamedata();
    tasker_free(g_tasker);
    term_uninit();
    ecs_uninit();
    eventing_uninit();
    uninit_logs();
}
