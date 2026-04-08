#include "player.h"

#include "utils.h"

player_t player = {
    .x = (MAP_NUM_COLS * TILE_SIZE) / 2.0,
    .y = (MAP_NUM_ROWS * TILE_SIZE) / 2.0,
    .width = 10,
    .height = 10,
    .turn_direction = 0,
    .walk_direction = 0,
    .rotation_angle = PI / 2,
    .walk_speed = 100,
    .turn_speed = 45 * (PI / 180),
};

void move_player(float delta_time) {
    player.rotation_angle +=
        player.turn_direction * player.turn_speed * delta_time;

    normalize_angle(&player.rotation_angle);

    float move_step = player.walk_direction * player.walk_speed * delta_time;

    float new_player_x = player.x + cos(player.rotation_angle) * move_step;
    float new_player_y = player.y + sin(player.rotation_angle) * move_step;

    if (!has_wall_at(new_player_x, player.y)) {
        player.x = new_player_x;
    }

    if (!has_wall_at(player.x, new_player_y)) {
        player.y = new_player_y;
    }
}

void render_map_player(void) {
    draw_rect(
        MINIMAP_SCALE_FACTOR * player.x,
        MINIMAP_SCALE_FACTOR * player.y,
        MINIMAP_SCALE_FACTOR * player.width,
        MINIMAP_SCALE_FACTOR * player.height,
        0xFFFFFFFF
    );
}
