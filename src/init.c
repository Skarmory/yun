#include "init.h"
#include "colour.h"
#include "command_defs.h"
#include "command_manager.h"
#include "gameplay.h"
#include "globals.h"
#include "log.h"
#include "mon_attack.h"
#include "mon_type.h"
#include "obj_armour.h"
#include "obj_weapon.h"
#include "parsing.h"
#include "symbol.h"
#include "tasking.h"
#include "util.h"

#include <stddef.h>
#include <stdlib.h>
#include <time.h>

static inline bool _init_symbols(void)
{
    g_symbol_armour_cloth.fg = *COL(g_symbol_armour_cloth.base_fg_idx);
    g_symbol_armour_cloth.bg = *COL(g_symbol_armour_cloth.base_bg_idx);

    g_symbol_armour_leather.fg = *COL(g_symbol_armour_leather.base_fg_idx);
    g_symbol_armour_leather.bg = *COL(g_symbol_armour_leather.base_bg_idx);

    g_symbol_armour_mail.fg = *COL(g_symbol_armour_mail.base_fg_idx);
    g_symbol_armour_mail.bg = *COL(g_symbol_armour_mail.base_bg_idx);

    g_symbol_armour_plate.fg = *COL(g_symbol_armour_plate.base_fg_idx);
    g_symbol_armour_plate.bg = *COL(g_symbol_armour_plate.base_bg_idx);

    g_symbol_weapon_metal.fg = *COL(g_symbol_weapon_metal.base_fg_idx);
    g_symbol_weapon_metal.bg = *COL(g_symbol_weapon_metal.base_bg_idx);

    g_symbol_weapon_wood.fg = *COL(g_symbol_weapon_wood.base_fg_idx);
    g_symbol_weapon_wood.bg = *COL(g_symbol_weapon_wood.base_bg_idx);

    return true;
}

static inline bool _init_gamedata(void)
{
    log_msg(DEBUG, "parsing attack types");
    if(parse_attack_methods() != PARSER_OK)
    {
        log_msg(DEBUG, "parsing failed");
        return false;
    }
    log_msg(DEBUG, "parsing complete");

    log_msg(DEBUG, "parsing armours");
    if(parse_armours() != PARSER_OK)
    {
        log_msg(DEBUG, "parsing failed");
        return false;
    }
    log_msg(DEBUG, "parsing complete");

    log_msg(DEBUG, "parsing weapons");
    if(parse_weapons() != PARSER_OK)
    {
        log_msg(DEBUG, "parsing failed");
        return false;
    }
    log_msg(DEBUG, "parsing complete");

    log_msg(DEBUG, "parsing mon types");
    if(parse_mon_types() != PARSER_OK)
    {
        log_msg(DEBUG, "parsing failed");
        return false;
    }
    log_msg(DEBUG, "parsing complete");

    return true;
}

void _uninit_gamedata(void)
{
    for(int idx = 0; idx < g_mon_type_count; ++idx)
        free(g_mon_type[idx].symbol);

    free(g_mon_type);
    g_mon_type_count = 0;

    free(g_weapon_base);
    g_weapon_base_count = 0;

    free(g_armour_base);
    g_armour_base_count = 0;

    free(g_attack_methods);
    g_attack_methods_count = 0;
}

static void _init_command_handling(void)
{
    g_command_manager = command_manager_new();

    command_manager_register_handler(g_command_manager, COMMAND_TYPE_MOVE, gameplay_command_handler_func);
    command_manager_register_handler(g_command_manager, COMMAND_TYPE_PASS_TURN, gameplay_command_handler_func);
    command_manager_register_handler(g_command_manager, COMMAND_TYPE_PICK_UP, gameplay_command_handler_func);
    command_manager_register_handler(g_command_manager, COMMAND_TYPE_DISPLAY_POSITION, gameplay_command_handler_func);
    //command_manager_register_handler(COMMAND_TYPE_DISPLAY_CHARACTER_SCREEN,
    //command_manager_register_handler(COMMAND_TYPE_DISPLAY_INVENTORY,
}

static void _uninit_command_handling(void)
{
    command_manager_free(g_command_manager);
}

bool init_naxx(void)
{
    srand(time(NULL));

    init_logs();
    term_init();
    term_set_sigint_callback(&sigint_handler);
    term_get_wh(&screen_cols, &screen_rows);

    g_tasker = tasker_new();

    if(!_init_symbols())
        return false;
    if(!_init_gamedata())
        return false;

    _init_command_handling();

    return true;
}

void uninit_naxx(void)
{
    _uninit_command_handling();
    _uninit_gamedata();
    tasker_free(g_tasker);
    term_uninit();
    uninit_logs();
}
