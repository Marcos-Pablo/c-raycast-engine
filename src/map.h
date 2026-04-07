#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

#include "base_defs.h"
#include "graphics.h"

#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20

bool has_wall_at(float x, float y);
bool is_inside_map(float x, float y);
void render_map(void);
int get_map_at(int i, int j);

#endif
