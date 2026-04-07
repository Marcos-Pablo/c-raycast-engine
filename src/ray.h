#ifndef RAY_H
#define RAY_H

#include <stdbool.h>

#include "base_defs.h"
#include "map.h"
#include "player.h"

typedef struct Ray {
    float ray_angle;
    float wall_hit_x;
    float wall_hit_y;
    float distance;
    bool was_hit_vertical;
    int wall_hit_content;
} ray_t;

extern ray_t rays[NUM_RAYS];

void cast_ray(int strip_id, float ray_angle);
void cast_all_rays(void);
void render_rays(void);

float normalize_angle(float angle);
float distance_between_points(float x1, float y1, float x2, float y2);

#endif
