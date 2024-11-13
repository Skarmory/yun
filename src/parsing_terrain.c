#include "parsing.h"

#include "terrain.h"

#include <scieppend/core/colour.h>
#include <scieppend/core/hash.h>
#include <scieppend/core/log.h>
#include <scieppend/core/symbol.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* c_terrains_file_name = "data/terrain.txt";

parsing_callback(_parse_terrain_id_callback);
parsing_callback(_parse_terrain_name_callback);
parsing_callback(_parse_terrain_desc_callback);
parsing_callback(_parse_terrain_symbol_callback);
parsing_callback(_parse_terrain_colour_callback);
parsing_callback(_parse_terrain_pathing_flags_callback);

enum ParserCode parse_terrains(void)
{
    struct Parser* parser = parser_new();

    parser_register_field(parser, "id", "id string", &_parse_terrain_id_callback);
    parser_register_field(parser, "name", "name string", &_parse_terrain_name_callback);
    parser_register_field(parser, "desc", "desc string", &_parse_terrain_desc_callback);
    parser_register_field(parser, "symbol", "symbol char", &_parse_terrain_symbol_callback);
    parser_register_field(parser, "colour", "fg_red int fg_green int fg_blue int bg_red int bg_green int bg_blue int", &_parse_terrain_colour_callback);
    parser_register_field(parser, "path-flag", "flag string", &_parse_terrain_pathing_flags_callback);

    if(parser_parse_file(parser, c_terrains_file_name))
    {
        terrain_parsing_finalise(parser);
    }

    enum ParserCode retval = parser_get_last_code(parser);
    parser_free(parser);
    return retval;
}

parsing_callback(_parse_terrain_id_callback)
{
    struct Terrain* terrain = malloc(sizeof(struct Terrain));
    memset(terrain , 0, sizeof(struct Terrain));

    parser_set_userdata(parser, terrain);
    snprintf(terrain->id, sizeof(terrain->id), "%s", parser_field_get_string(parser, "id", "id"));
    terrain->id_hash = hash(terrain->id, sizeof(terrain->id));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_terrain_name_callback)
{
    struct Terrain* terrain = parser_get_userdata_active(parser);
    snprintf(terrain->name, sizeof(terrain->name), "%s", parser_field_get_string(parser, "name", "name"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_terrain_desc_callback)
{
    struct Terrain* terrain = parser_get_userdata_active(parser);
    snprintf(terrain->desc, sizeof(terrain->desc), "%s", parser_field_get_string(parser, "desc", "desc"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_terrain_symbol_callback)
{
    struct Terrain* terrain = parser_get_userdata_active(parser);
    terrain->symbol = malloc(sizeof(struct Symbol));
    memset(terrain->symbol, 0, sizeof(struct Symbol));

    terrain->symbol->sym = parser_field_get_char(parser, "symbol", "symbol");
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_terrain_colour_callback)
{
    struct Terrain* terrain = parser_get_userdata_active(parser);
    terrain->symbol->fg.r = parser_field_get_int(parser, "colour", "fg_red");
    terrain->symbol->fg.g = parser_field_get_int(parser, "colour", "fg_green");
    terrain->symbol->fg.b = parser_field_get_int(parser, "colour", "fg_blue");
    terrain->symbol->bg.r = parser_field_get_int(parser, "colour", "bg_red");
    terrain->symbol->bg.g = parser_field_get_int(parser, "colour", "bg_green");
    terrain->symbol->bg.b = parser_field_get_int(parser, "colour", "bg_blue");
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_terrain_pathing_flags_callback)
{
    struct Terrain* terrain = parser_get_userdata_active(parser);
    terrain->pathing_flags |= pathing_path_flag_from_string(parser_field_get_string(parser, "path-flag", "flag"));
    return PARSE_CALLBACK_OK;
}
