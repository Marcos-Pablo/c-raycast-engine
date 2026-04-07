#include "map.h"

static const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5}
};

bool has_wall_at(float x, float y) {
    int map_x = floor(x / TILE_SIZE);
    int map_y = floor(y / TILE_SIZE);

    if (map_x < 0 || map_x >= MAP_NUM_COLS) return true;
    if (map_y < 0 || map_y >= MAP_NUM_ROWS) return true;
    return map[map_y][map_x] != 0;
}

void render_map() {
    for (int i = 0; i < MAP_NUM_ROWS; i++) {
        for (int j = 0; j < MAP_NUM_COLS; j++) {
            int tile_x = j * TILE_SIZE;
            int tile_y = i * TILE_SIZE;
            u32 tile_color = map[i][j] != 0 ? 0xFFFFFFFF : 0x00000000;

            draw_rect(
                MINIMAP_SCALE_FACTOR * tile_x,
                MINIMAP_SCALE_FACTOR * tile_y,
                MINIMAP_SCALE_FACTOR * TILE_SIZE,
                MINIMAP_SCALE_FACTOR * TILE_SIZE,
                tile_color
            );
        }
    }
}

int get_map_at(int i, int j) {
    // get wall content
    return map[i][j];
}

bool is_inside_map(float x, float y) {
    return x >= 0 && x <= MAP_NUM_COLS * TILE_SIZE && y >= 0 &&
           y <= MAP_NUM_ROWS * TILE_SIZE;
}
