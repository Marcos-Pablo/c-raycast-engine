#include "ray.h"

#include "graphics.h"

ray_t rays[NUM_RAYS];

void normalize_angle(float* angle) {
    *angle = remainder(*angle, TWO_PI);

    if (*angle < 0) {
        *angle += TWO_PI;
    }
}

float distance_between_points(float x1, float y1, float x2, float y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool is_ray_facing_down(float angle) {
    // In our coordinate system, the ray is facing down if its angle is between 0 and PI radians (180 degrees)
    return angle > 0 && angle < PI;
}
bool is_ray_facing_up(float angle) {
    // If the ray is not facing down, then it is facing up
    return !is_ray_facing_down(angle);
}
bool is_ray_facing_right(float angle) {
    // In our coordinate system, the ray is facing right if its angle is between 0 and 90 degrees (0 and PI/2 radians) or between 270 and 360 degrees (3PI/2 and TWO_PI radians)
    return angle < 0.5 * PI || angle > 1.5 * PI;
}
bool is_ray_facing_left(float angle) {
    // If the ray is not facing right, then it is facing left
    return !is_ray_facing_right(angle);
}

void cast_ray(int strip_id, float ray_angle) {
    normalize_angle(&ray_angle);

    float yintercept, xintercept;
    float xstep, ystep;

    /////////////////////////////////////////////////////////////////////////
    // HORIZONTAL RAY-GRID INTERSECTION CODE
    /////////////////////////////////////////////////////////////////////////
    bool found_horz_wall_hit = false;
    float horz_wall_hit_x = 0;
    float horz_wall_hit_y = 0;
    int horz_wall_content = 0;

    // Find the y-coordinate of the closest horizontal grid intersetion
    yintercept = floor(player.y / TILE_SIZE) * TILE_SIZE;
    yintercept += is_ray_facing_down(ray_angle) ? TILE_SIZE : 0;

    // Find the x-coordinate of the closest horizontal grid intersetion
    xintercept = player.x + (yintercept - player.y) / tan(ray_angle);

    // Calculate the increment xstep and ystep
    ystep = TILE_SIZE;
    ystep *= is_ray_facing_up(ray_angle) ? -1 : 1;

    xstep = TILE_SIZE / tan(ray_angle);
    xstep *= (is_ray_facing_left(ray_angle) && xstep > 0) ? -1 : 1;
    xstep *= (is_ray_facing_right(ray_angle) && xstep < 0) ? -1 : 1;

    float next_horz_touch_x = xintercept;
    float next_horz_touch_y = yintercept;

    // Increment x_step and y_step until we find a wall
    while (is_inside_map(next_horz_touch_x, next_horz_touch_y)) {
        float x_to_check = next_horz_touch_x;
        float y_to_check =
            next_horz_touch_y + (is_ray_facing_up(ray_angle) ? -1 : 0);

        if (has_wall_at(x_to_check, y_to_check)) {
            found_horz_wall_hit = true;
            horz_wall_hit_x = next_horz_touch_x;
            horz_wall_hit_y = next_horz_touch_y;
            horz_wall_content = get_map_at(
                (int)floor(y_to_check / TILE_SIZE),
                (int)floor(x_to_check / TILE_SIZE)
            );
            break;
        }
        next_horz_touch_x += xstep;
        next_horz_touch_y += ystep;
    }

    /////////////////////////////////////////////////////////////////////////
    // VERTICAL RAY-GRID INTERSECTION CODE
    /////////////////////////////////////////////////////////////////////////

    bool found_vert_wall_hit = false;
    float vert_wall_hit_x = 0;
    float vert_wall_hit_y = 0;
    int vert_wall_content = 0;

    // Find the x-coordinate of the closest vertical grid intersetion
    xintercept = floor(player.x / TILE_SIZE) * TILE_SIZE;
    xintercept += is_ray_facing_right(ray_angle) ? TILE_SIZE : 0;

    // Find the y-coordinate of the closest vertical grid intersetion
    yintercept = player.y + (xintercept - player.x) * tan(ray_angle);

    // Calculate the increment xstep and ystep
    xstep = TILE_SIZE;
    xstep *= is_ray_facing_left(ray_angle) ? -1 : 1;

    ystep = TILE_SIZE * tan(ray_angle);
    ystep *= (is_ray_facing_up(ray_angle) && ystep > 0) ? -1 : 1;
    ystep *= (is_ray_facing_down(ray_angle) && ystep < 0) ? -1 : 1;

    float next_vert_touch_x = xintercept;
    float next_vert_touch_y = yintercept;

    // Increment x_step and y_step until we find a wall
    while (is_inside_map(next_vert_touch_x, next_vert_touch_y)) {
        float x_to_check =
            next_vert_touch_x + (is_ray_facing_left(ray_angle) ? -1 : 0);
        float y_to_check = next_vert_touch_y;

        if (has_wall_at(x_to_check, y_to_check)) {
            found_vert_wall_hit = true;
            vert_wall_hit_x = next_vert_touch_x;
            vert_wall_hit_y = next_vert_touch_y;
            vert_wall_content = get_map_at(
                (int)floor(y_to_check / TILE_SIZE),
                (int)floor(x_to_check / TILE_SIZE)
            );
            break;
        }
        next_vert_touch_x += xstep;
        next_vert_touch_y += ystep;
    }

    // Calculate both horizontal and vertical distances and choose the smallest value
    float horz_hit_distance =
        found_horz_wall_hit
            ? distance_between_points(
                  player.x, player.y, horz_wall_hit_x, horz_wall_hit_y
              )
            : FLT_MAX;

    float vert_hit_distance =
        found_vert_wall_hit
            ? distance_between_points(
                  player.x, player.y, vert_wall_hit_x, vert_wall_hit_y
              )
            : FLT_MAX;

    if (vert_hit_distance < horz_hit_distance) {
        rays[strip_id].distance = vert_hit_distance;
        rays[strip_id].wall_hit_x = vert_wall_hit_x;
        rays[strip_id].wall_hit_y = vert_wall_hit_y;
        rays[strip_id].wall_hit_content = vert_wall_content;
        rays[strip_id].was_hit_vertical = true;
    } else {
        rays[strip_id].distance = horz_hit_distance;
        rays[strip_id].wall_hit_x = horz_wall_hit_x;
        rays[strip_id].wall_hit_y = horz_wall_hit_y;
        rays[strip_id].wall_hit_content = horz_wall_content;
        rays[strip_id].was_hit_vertical = false;
    }

    rays[strip_id].ray_angle = ray_angle;
}

void cast_all_rays(void) {
    for (int col = 0; col < NUM_RAYS; col++) {
        float ray_angle = player.rotation_angle +
                          atan((col - NUM_RAYS / 2) / DIST_PROJ_PLANE);
        cast_ray(col, ray_angle);
    }
}

void render_map_rays(void) {
    for (int i = 0; i < NUM_RAYS; i++) {
        draw_line(
            MINIMAP_SCALE_FACTOR * player.x,
            MINIMAP_SCALE_FACTOR * player.y,
            MINIMAP_SCALE_FACTOR * rays[i].wall_hit_x,
            MINIMAP_SCALE_FACTOR * rays[i].wall_hit_y,
            0xFF0000FF
        );
    }
}
