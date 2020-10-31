#ifndef YUN_PARSING_H
#define YUN_PARSING_H

#include "parser.h"

#define parsing_callback(func_name) static enum ParseCallbackCode func_name(struct Parser* parser)

enum ParserCode parse_attack_methods(void);
enum ParserCode parse_mon_types(void);
enum ParserCode parse_armours(void);
enum ParserCode parse_weapons(void);
enum ParserCode parse_features(void);

#endif
