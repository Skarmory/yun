#include "init.h"
#include "globals.h"
#include "log.h"
#include "parsing.h"
#include "util.h"

#include <stddef.h>
#include <stdlib.h>
#include <time.h>

// TEMP REMOVE THESE
#include "obj_armour.h"
#include "mon_type.h"
#include "symbol.h"

bool init_naxx(void)
{
    srand(time(NULL));

    init_logs();
    term_init();
    term_set_sigint_callback(&sigint_handler);
    term_get_wh(&screen_cols, &screen_rows);

    log_msg(DEBUG, "parsing armours");
    if(parse_armours() != PARSER_OK)
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

    init_symbols();
    init_montypes();

    return true;
}

void uninit_naxx(void)
{
    uninit_logs();
    uninit_montypes();

    free(g_armour_base);
    g_armour_base_count = 0;

    term_uninit();
}
