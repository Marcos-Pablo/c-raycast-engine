#include "wall.h"

void render_wall_projection(void) {
    for (int x = 0; x < NUM_RAYS; x++) {
        float perpendicular_distance =
            rays[x].distance * cos(rays[x].ray_angle - player.rotation_angle);

        float projected_wall_height =
            (TILE_SIZE / perpendicular_distance) * DIST_PROJ_PLANE;

        int wall_strip_height = (int)projected_wall_height;
        int wall_top_pixel = (WINDOW_HEIGHT / 2) - (wall_strip_height / 2);
        wall_top_pixel = wall_top_pixel < 0 ? 0 : wall_top_pixel;

        int wall_bottom_pixel = (WINDOW_HEIGHT / 2) + (wall_strip_height / 2);
        wall_bottom_pixel = wall_bottom_pixel > WINDOW_HEIGHT
                                ? WINDOW_HEIGHT
                                : wall_bottom_pixel;

        // set the color of the ceiling
        for (int y = 0; y < wall_top_pixel; y++) {
            draw_pixel(x, y, 0xFF333333);
        }

        int texture_offset_x;

        if (rays[x].was_hit_vertical) {
            texture_offset_x = (int)rays[x].wall_hit_y % TILE_SIZE;
        } else {
            texture_offset_x = (int)rays[x].wall_hit_x % TILE_SIZE;
        }

        int tex_num = rays[x].wall_hit_content - 1;
        int texture_with = wall_textures[tex_num].width;
        int texture_height = wall_textures[tex_num].height;

        for (int y = wall_top_pixel; y < wall_bottom_pixel; y++) {
            int distance_from_top =
                y + (wall_strip_height / 2) - (WINDOW_HEIGHT / 2);

            int texture_offset_y =
                distance_from_top * ((float)texture_with / wall_strip_height);

            color_t texel_color =
                wall_textures[tex_num].texture_buffer
                    [(texture_height * texture_offset_y) + texture_offset_x];

            // Set the color of the wall based on the color from the texture
            draw_pixel(x, y, texel_color);
        }

        // set the color of the floor
        for (int y = wall_bottom_pixel; y < WINDOW_HEIGHT; y++) {
            draw_pixel(x, y, 0xFF777777);
        }
    }
}
