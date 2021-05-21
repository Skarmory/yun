#ifndef YUN_LOOK_H
#define YUN_LOOK_H

#include "input_keycodes.h"
#include "symbol.h"

struct MapLocation;
struct Mon;

/* Enter cursor free move mode.
 * Has the logic for moving the cursor a valid move in any direction.
 * Is not a sub-state: designed to be sequentially called and specific logic implemented by the caller performed at each valid move.
 *
 * Params:
 *      in_loc: Starting location for the free move
 *      out_loc: Ending location for the free move
 *
 * Return:
 *      The *valid* keycode that the user last input
 */
enum KeyCode cursor_free_move(struct MapLocation* restrict in_loc, struct MapLocation** restrict out_loc);

struct Symbol look_get_symbol(const struct MapLocation* loc, const struct Mon* looker);
void look(void);

#endif
