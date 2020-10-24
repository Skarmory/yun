#ifndef NAXX_INPUT_KEYCODES_H
#define NAXX_INPUT_KEYCODES_H

enum KeyCode
{
    KEYCODE_UNKNOWN                 = -1,
    KEYCODE_ENTER                   = 13,
    KEYCODE_ESC                     = 27,

    KEYCODE_CHAR_RANGE_START        = 32,
    KEYCODE_SPACE                   = 32,
    KEYCODE_EXCLAMATION             = 33,
    KEYCODE_QUOTE                   = 34,
    KEYCODE_HASH                    = 35,
    KEYCODE_DOLLAR                  = 36,
    KEYCODE_PERCENT                 = 37,
    KEYCODE_AMPERSAND               = 38,
    KEYCODE_APOSTROPHE              = 39,
    KEYCODE_OPEN_PARENTHESIS        = 40,
    KEYCODE_CLOSE_PARENTHESIS       = 41,
    KEYCODE_ASTERISK                = 42,
    KEYCODE_PLUS                    = 43,
    KEYCODE_COMMA                   = 44,
    KEYCODE_HYPHEN                  = 45,
    KEYCODE_DOT                     = 46,
    KEYCODE_FORWARD_SLASH           = 47,
    KEYCODE_0                       = 48,
    KEYCODE_1                       = 49,
    KEYCODE_2                       = 50,
    KEYCODE_3                       = 51,
    KEYCODE_4                       = 52,
    KEYCODE_5                       = 53,
    KEYCODE_6                       = 54,
    KEYCODE_7                       = 55,
    KEYCODE_8                       = 56,
    KEYCODE_9                       = 57,
    KEYCODE_COLON                   = 58,
    KEYCODE_SEMICOLON               = 59,
    KEYCODE_LESS_THAN               = 60,
    KEYCODE_EQUALS                  = 61,
    KEYCODE_GREATER_THAN            = 62,
    KEYCODE_QUESTION                = 63,
    KEYCODE_AT                      = 64,
    KEYCODE_A                       = 65,
    KEYCODE_B                       = 66,
    KEYCODE_C                       = 67,
    KEYCODE_D                       = 68,
    KEYCODE_E                       = 69,
    KEYCODE_F                       = 70,
    KEYCODE_G                       = 71,
    KEYCODE_H                       = 72,
    KEYCODE_I                       = 73,
    KEYCODE_J                       = 74,
    KEYCODE_K                       = 75,
    KEYCODE_L                       = 76,
    KEYCODE_M                       = 77,
    KEYCODE_N                       = 78,
    KEYCODE_O                       = 79,
    KEYCODE_P                       = 80,
    KEYCODE_Q                       = 81,
    KEYCODE_R                       = 82,
    KEYCODE_S                       = 83,
    KEYCODE_T                       = 84,
    KEYCODE_U                       = 85,
    KEYCODE_V                       = 86,
    KEYCODE_W                       = 87,
    KEYCODE_X                       = 88,
    KEYCODE_Y                       = 89,
    KEYCODE_Z                       = 90,
    KEYCODE_OPEN_SQUARE             = 91,
    KEYCODE_BACKSLASH               = 92,
    KEYCODE_CLOSE_SQUARE            = 93,
    KEYCODE_CARET                   = 94,
    KEYCODE_UNDERSCORE              = 95,
    KEYCODE_BACKTICK                = 96,
    KEYCODE_a                       = 97,
    KEYCODE_b                       = 98,
    KEYCODE_c                       = 99,
    KEYCODE_d                       = 100,
    KEYCODE_e                       = 101,
    KEYCODE_f                       = 102,
    KEYCODE_g                       = 103,
    KEYCODE_h                       = 104,
    KEYCODE_i                       = 105,
    KEYCODE_j                       = 106,
    KEYCODE_k                       = 107,
    KEYCODE_l                       = 108,
    KEYCODE_m                       = 109,
    KEYCODE_n                       = 110,
    KEYCODE_o                       = 111,
    KEYCODE_p                       = 112,
    KEYCODE_q                       = 113,
    KEYCODE_r                       = 114,
    KEYCODE_s                       = 115,
    KEYCODE_t                       = 116,
    KEYCODE_u                       = 117,
    KEYCODE_v                       = 118,
    KEYCODE_w                       = 119,
    KEYCODE_x                       = 120,
    KEYCODE_y                       = 121,
    KEYCODE_z                       = 122,
    KEYCODE_OPEN_CURLY              = 123,
    KEYCODE_CURLY_PIPE              = 124,
    KEYCODE_CLOSE_CURLY             = 125,
    KEYCODE_TILDE                   = 126,
    KEYCODE_CHAR_RANGE_END          = 126,

    KEYCODE_SPECIAL_RANGE_START     = 257,
    KEYCODE_ARROW_UP                = 257,
    KEYCODE_ARROW_DOWN              = 258,
    KEYCODE_ARROW_RIGHT             = 259,
    KEYCODE_ARROW_LEFT              = 260,
    KEYCODE_SPECIAL_RANGE_END       = 260

    //TODO: Non-ascii, figure out how to handle?
    //KEYCODE_C_CEDILLA               = 128,
    //KEYCODE_u_UMLAUT                = 129,
    //KEYCODE_e_ACUTE                 = 130,
    //KEYCODE_a_CIRCUMFLEX            = 131,
    //KEYCODE_a_UMLAUT                = 132,
    //KEYCODE_a_GRAVE                 = 133,
    //KEYCODE_a_OVERRING              = 134,
    //KEYCODE_c_CEDILLA               = 135,
    //KEYCODE_e_CIRCUMFLEX            = 136,
    //KEYCODE_e_UMLAUT                = 137,
    //KEYCODE_e_GRAVE                 = 138,
    //KEYCODE_i_UMLAUT                = 139,
    //KEYCODE_i_CIRCUMFLEX            = 140,
    //KEYCODE_i_GRAVE                 = 141,
    //KEYCODE_A_UMLAUT                = 142,
    //KEYCODE_A_OVERRING              = 143,
    //KEYCODE_E_ACUTE                 = 144,
    //KEYCODE_ae                      = 145,
    //KEYCODE_AE                      = 146,
    //KEYCODE_o_CIRCUMFLEX            = 147,
    //KEYCODE_o_UMLAUT                = 148,
    //KEYCODE_o_GRAVE                 = 149,
    //KEYCODE_u_CIRCUMFLEX            = 150,
    //KEYCODE_u_GRAVE                 = 151,
    //KEYCODE_y_UMLAUT                = 152,
    //KEYCODE_O_UMLAUT                = 153,
    //KEYCODE_U_UMLAUT                = 154,
    //KEYCODE_o_SLASHED               = 155,
    //KEYCODE_POUND                   = 156,
    //KEYCODE_O_SLASHED               = 157,
    //KEYCODE_MULTIPLY                = 158,
    //KEYCODE_FUNCTION                = 159,
    //KEYCODE_a_ACUTE                 = 160,
    //KEYCODE_i_ACUTE                 = 161,
    //KEYCODE_o_ACUTE                 = 162,
    //KEYCODE_u_ACUTE                 = 163,
    //KEYCODE_n_TILDE                 = 164,
    //KEYCODE_N_TILDE                 = 165,
    //KEYCODE_UPSIDE_DOWN_QUESTION    = 168,
    //KEYCODE_UPSIDE_DOWN_EXCLAMATION = 173,
    //KEYCODE_OPEN_ANGLED             = 174,
    //KEYCODE_CLOSE_ANGLED            = 175,
    //KEYCODE_A_ACUTE                 = 181,
    //KEYCODE_A_CIRCUMFLEX            = 182,
    //KEYCODE_A_GRAVE                 = 183,
    //KEYCODE_CENT                    = 189,
    //KEYCODE_YEN                     = 190,
    //KEYCODE_a_TILDE                 = 198,
    //KEYCODE_A_TILDE                 = 199,
    //KEYCODE_ETH_LOWER               = 208,
    //KEYCODE_ETH_UPPER               = 209,
    //KEYCODE_E_CIRCUMFLEX            = 210,
    //KEYCODE_E_UMLAUT                = 211,
    //KEYCODE_E_GRAVE                 = 212,
    //KEYCODE_i_NO_DOT                = 213,
    //KEYCODE_I_ACUTE                 = 214,
    //KEYCODE_I_CIRCUMFLEX            = 215,
    //KEYCODE_I_UMLAUT                = 216,
    //KEYCODE_I_GRAVE                 = 222,
    //KEYCODE_O_ACUTE                 = 224,
    //KEYCODE_ESZETT                  = 225,
    //KEYCODE_O_CIRCUMFLEX            = 226,
    //KEYCODE_O_GRAVE                 = 227,
    //KEYCODE_o_TILDE                 = 228,
    //KEYCODE_O_TILDE                 = 229,
    //KEYCODE_MU                      = 230,
    //KEYCODE_THORN_LOWER             = 231,
    //KEYCODE_THORN_UPPER             = 232,
    //KEYCODE_U_ACUTE                 = 233,
    //KEYCODE_U_CIRCUMFLEX            = 234,
    //KEYCODE_U_GRAVE                 = 235,
    //KEYCODE_y_ACUTE                 = 236,
    //KEYCODE_Y_ACUTE                 = 237
};

enum KeyCode get_key(void);

#endif
