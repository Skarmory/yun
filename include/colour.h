#ifndef COLOUR_H
#define COLOUR_H

// colours
// base
#define CLR_BLACK   0
#define CLR_RED     1
#define CLR_GREEN   2
#define CLR_YELLOW  3
#define CLR_BLUE    4
#define CLR_MAGENTA 5
#define CLR_CYAN    6
#define CLR_WHITE   7
#define CLR_ORANGE  8
#define CLR_PINK    9
#define CLR_BROWN   10
#define CLR_PURPLE  11

// colour pairs
// base

#define CP_BLACK_BLACK   0
#define CP_RED_BLACK     1
#define CP_GREEN_BLACK   2
#define CP_YELLOW_BLACK  3
#define CP_BLUE_BLACK    4
#define CP_MAGENTA_BLACK 5
#define CP_CYAN_BLACK    6
#define CP_WHITE_BLACK   7
#define CP_ORANGE_BLACK  8
#define CP_PINK_BLACK    9
#define CP_BROWN_BLACK   10
#define CP_PURPLE_BLACK  11

// specific
#define CP_DEFAULT  CP_WHITE_BLACK 
#define CP_ALLIANCE CP_BLUE_BLACK
#define CP_HORDE    CP_RED_BLACK

#define CP_SHAMAN   CP_BLUE_BLACK
#define CP_DRUID    CP_ORANGE_BLACK
#define CP_PALADIN  CP_PINK_BLACK
#define CP_WARRIOR  CP_BROWN_BLACK
#define CP_WARLOCK  CP_PURPLE_BLACK 
#define CP_PRIEST   CP_WHITE_BLACK 
#define CP_DRUID    CP_ORANGE_BLACK  
#define CP_ROGUE    CP_YELLOW_BLACK 
#define CP_HUNTER   CP_GREEN_BLACK 
#define CP_MAGE     CP_CYAN_BLACK


void init_colours(void);

#endif
