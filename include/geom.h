#ifndef YUN_GEOM_H
#define YUN_GEOM_H

#include "list.h"

#include <stdbool.h>

struct Coordinate
{
    int x;
    int y;
};

struct Line
{
    struct List coordinate_list;
};

/* 
 * Get the next square along in a line from (x0, y0) to (x1, y1)
 * Puts the next square's (x, y) in the given pointers
 */
bool geom_gen_line_increment(int x0, int y0, int x1, int y1, int* x, int* y, float* err);

void geom_gen_line(struct Line* out_line, int x0, int y0, int x1, int y1);

/*
 * Check whether (px, py) is inside circle centred at (cx, cy) with radius r
 */
bool geom_point_in_circle(int px, int py, int cx, int cy, int r);

/*
 * Check whether (px, py) is inside rectangle with top left corner at (rx, ry) with width w and height h
 */
bool geom_point_in_rect(int px, int py, int rx, int ry, int w, int h);

void geom_dbg_log_line(struct Line* line, const char* line_name);

#endif
