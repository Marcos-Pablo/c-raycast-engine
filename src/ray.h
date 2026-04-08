#ifndef RAY_H
#define RAY_H

#include <limits.h>
#include <stdbool.h>

#include "base_defs.h"
#include "map.h"
#include "player.h"
#include "utils.h"

typedef struct Ray {
    float ray_angle;
    float wall_hit_x;
    float wall_hit_y;
    float distance;
    bool was_hit_vertical;
    int texture;
} ray_t;

extern ray_t rays[NUM_RAYS];

void cast_ray(int strip_id, float ray_angle);
void cast_all_rays(void);
void render_map_rays(void);

bool is_ray_facing_up(float angle);
bool is_ray_facing_down(float angle);
bool is_ray_facing_left(float angle);
bool is_ray_facing_right(float angle);

#endif
