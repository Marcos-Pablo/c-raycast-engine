#ifndef PLAYER_H
#define PLAYER_H

#include "base_defs.h"
#include "graphics.h"
#include "map.h"

typedef struct {
    float x;
    float y;
    float width;
    float height;
    float rotation_angle;
    float walk_speed;
    float turn_speed;
    int turn_direction;  // -1 for left, +1 for right
    int walk_direction;  // -1 for back, +1 for front
} player_t;

extern player_t player;

void move_player(float delta_time);
void render_player(void);

#endif
