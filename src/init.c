#include "init.h"
#include "globals.h"
#include "log.h"
#include "parsing.h"
#include "util.h"

#include <stddef.h>
#include <stdlib.h>
#include <time.h>

// TEMP REMOVE THESE
#include "mon_type.h"
#include "symbol.h"

bool init_naxx(void)
{
    srand(time(NULL));

    init_logs();
    term_init();
    term_set_sigint_callback(&sigint_handler);
    term_get_wh(&screen_cols, &screen_rows);

    parse_mon_types();
    init_symbols();
    init_montypes();

    return true;
}

void uninit_naxx(void)
{
    uninit_logs();
    uninit_montypes();
    term_uninit();
}
