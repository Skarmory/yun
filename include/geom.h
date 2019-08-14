#ifndef NAXX_GEOM_H
#define NAXX_GEOM_H

#include <stdbool.h>

/* 
 * Get the next square along in a line from (x0, y0) to (x1, y1)
 * Puts the next square's (x, y) in the given pointers
 */
bool gen_line_increment(int x0, int y0, int x1, int y1, int* x, int* y, float* err);

#endif
