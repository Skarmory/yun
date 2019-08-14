#include "geom.h"

#include <math.h>
#include <stdlib.h>


/*      
 *      \    |    /
 *       \ 6 | 7 /
 *        \  |  /
 *     5   \ | /   8
 *          \|/ 
 *   ------------------
 *          /|\
 *     4   / | \   1
 *        /  |  \
 *       / 3 | 2 \
 *      /    |    \
 *   
 *   
 *   1: dx > dy, +dx +dy
 *   8: dx > dy, +dx -dy
 *   
 *   2: dy > dx, +dx +dy
 *   7: dy > dx, +dx -dy
 *   
 *   3: dy > dx, -dx +dy
 *   6: dy > dx, -dx -dy
 *   
 *   4: dx > dy, -dx +dy
 *   5: dx > dy, -dx -dy
 */

static inline void _line_increment(int* x, int* y, float* err, float dx, float dy, float derr)
{
    (*x) += (int)((dx > 0.0f) - (dx < 0.0f));
    (*err) += derr;

    if(*err >= 0.5f)
    {
        (*y)   += (int)((dy > 0.0f) - (dy < 0.0f));
        (*err) -= 1.0f;
    }
}

bool geom_gen_line_increment(int x0, int y0, int x1, int y1, int* x, int* y, float* err)
{
    if(*x == x1 && *y == y1) return false;

    float dx   = (float)x1 - (float)x0;
    float dy   = (float)y1 - (float)y0;
    float derr = 0.5f;

    if(fabs(dx) >= fabs(dy))
    {
        derr = (dx == 0.0f ? 0.5f : fabs(dy / dx));
        _line_increment(x, y, err, dx, dy, derr);
    }
    else
    {
        derr = (dy == 0.0f ? 0.5f : fabs(dx / dy));
        _line_increment(y, x, err, dy, dx, derr);
    }

    return true;
}

bool geom_point_in_circle(int px, int py, int cx, int cy, int r)
{
    int dx = abs(px - cx);
    int dy = abs(py - cy);
    int len = (dx * dx) + (dy * dy);
    return len < (r*r);
}
