#ifndef NAXX_COLOUR_H
#define NAXX_COLOUR_H

// basic colours
#define CLR_BLACK   0
#define CLR_RED     1
#define CLR_GREEN   2
#define CLR_YELLOW  3
#define CLR_BLUE    4
#define CLR_PURPLE  5
#define CLR_CYAN    6
#define CLR_WHITE   7
#define CLR_DGREY   8
#define CLR_LRED    9
#define CLR_LGREEN  10
#define CLR_LYELLOW 11
#define CLR_LBLUE   12
#define CLR_LPURPLE 13
#define CLR_LCYAN   14
#define CLR_LWHITE  15

#define CLR_BROWN   94
#define CLR_ORANGE  214
#define CLR_DORANGE 208

// specific
#define CLR_DEFAULT  0
#define CLR_ALLIANCE 21
#define CLR_HORDE    196

#define CLR_SHAMAN   25
#define CLR_DRUID    208
#define CLR_PALADIN  218
#define CLR_WARRIOR  136
#define CLR_WARLOCK  13
#define CLR_PRIEST   255
#define CLR_ROGUE    191
#define CLR_HUNTER   113
#define CLR_MAGE     75

// mob difficulty class
// grey - green - blue - purple - yellow - orange - red
// This is more absolute difficulty than relative
// i.e. a grey 'g' could be a 'ghoul' (one of the weakest 'g') but a red 'g' could be an 'infectious ghoul' (one of the strongest 'g')
#define CLR_DC_1 CLR_DGREY
#define CLR_DC_2 CLR_LGREEN
#define CLR_DC_3 CLR_BLUE
#define CLR_DC_4 CLR_YELLOW
#define CLR_DC_5 CLR_ORANGE
#define CLR_DC_6 CLR_LRED
#define CLR_DC_7 CLR_LPURPLE

// No colour. Specify for things that should not be seen (e.g. bare fists as a weapon shouldn't be as a symbol)
#define CLR_NONE CLR_BLACK

// Objects
#define CLR_METAL CLR_WHITE

void init_colours(void);

#endif
