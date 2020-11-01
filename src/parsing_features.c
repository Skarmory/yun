#include "parsing.h"

#include "colour.h"
#include "feature.h"
#include "log.h"
#include "pathing_flags.h"
#include "symbol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* c_features_file_name = "data/features.txt";

parsing_callback(_parse_features_finalise);
parsing_callback(_parse_feature_id_callback);
parsing_callback(_parse_feature_name_callback);
parsing_callback(_parse_feature_desc_callback);
parsing_callback(_parse_feature_symbol_callback);
parsing_callback(_parse_feature_colour_callback);
parsing_callback(_parse_feature_pathing_flags_callback);
parsing_callback(_parse_feature_block_sight_callback);

enum ParserCode parse_features(void)
{
    struct Parser* parser = parser_new();

    parser_register_field(parser, "id", "id string", &_parse_feature_id_callback);
    parser_register_field(parser, "name", "name string", &_parse_feature_name_callback);
    parser_register_field(parser, "desc", "desc string", &_parse_feature_desc_callback);
    parser_register_field(parser, "symbol", "symbol char", &_parse_feature_symbol_callback);
    parser_register_field(parser, "colour", "red int green int blue int", &_parse_feature_colour_callback);
    parser_register_field(parser, "path-flag", "flag string", &_parse_feature_pathing_flags_callback);
    parser_register_field(parser, "block-sight", "value bool", &_parse_feature_block_sight_callback);

    if(open_file_and_parse_all(parser, c_features_file_name))
    {
        _parse_features_finalise(parser);
    }

    enum ParserCode retval = parser_get_last_code(parser);
    parser_free(parser);
    return retval;
}

parsing_callback(_parse_features_finalise)
{
    List* features_data = parser_get_userdata(parser);
    ListNode* node = NULL;

    log_format_msg(LOG_DEBUG, "Parsed features count: %d", features_data->count);

    g_features = malloc(sizeof(struct Feature) * features_data->count);
    g_features_count = features_data->count;

    int idx = 0;
    list_for_each(features_data, node)
    {
        memcpy(&g_features[idx++], node->data, sizeof(struct Feature));
        free(node->data);
    }

    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_feature_id_callback)
{
    struct Feature* feature = malloc(sizeof(struct Feature));
    memset(feature , 0, sizeof(struct Feature));

    parser_set_userdata(parser, feature);
    snprintf(feature->id, sizeof(feature->id), "%s", parser_field_get_string(parser, "id", "id"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_feature_name_callback)
{
    struct Feature* feature = parser_get_userdata_active(parser);
    snprintf(feature->name, sizeof(feature->name), "%s", parser_field_get_string(parser, "name", "name"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_feature_desc_callback)
{
    struct Feature* feature = parser_get_userdata_active(parser);
    snprintf(feature->desc, sizeof(feature->desc), "%s", parser_field_get_string(parser, "desc", "desc"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_feature_symbol_callback)
{
    struct Feature* feature = parser_get_userdata_active(parser);
    feature->symbol = malloc(sizeof(struct Symbol));
    memset(feature->symbol, 0, sizeof(struct Symbol));

    feature->symbol->sym = parser_field_get_char(parser, "symbol", "symbol");
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_feature_colour_callback)
{
    struct Feature* feature = parser_get_userdata_active(parser);
    feature->symbol->fg.r = parser_field_get_int(parser, "colour", "red");
    feature->symbol->fg.g = parser_field_get_int(parser, "colour", "green");
    feature->symbol->fg.b = parser_field_get_int(parser, "colour", "blue");
    feature->symbol->bg = *COL(CLR_DEFAULT);
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_feature_pathing_flags_callback)
{
    struct Feature* feature = parser_get_userdata_active(parser);
    feature->pathing_flags |= pathing_path_flag_from_string(parser_field_get_string(parser, "path-flag", "flag"));
    return PARSE_CALLBACK_OK;
}

parsing_callback(_parse_feature_block_sight_callback)
{
    struct Feature* feature = parser_get_userdata_active(parser);
    feature->block_sight = parser_field_get_bool(parser, "block-sight", "value");
    return PARSE_CALLBACK_OK;
}
