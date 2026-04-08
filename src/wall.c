#include "wall.h"

void render_wall_projection(void) {
    for (int x = 0; x < NUM_RAYS; x++) {
        // Correct the fish-eye distortion by using the perpendicular distance instead of the direct distance to the wall hit
        float perpendicular_distance =
            rays[x].distance * cos(rays[x].ray_angle - player.rotation_angle);

        float wall_height =
            (TILE_SIZE / perpendicular_distance) * DIST_PROJ_PLANE;

        int wall_top_y = (WINDOW_HEIGHT / 2) - (wall_height / 2);
        wall_top_y = wall_top_y < 0 ? 0 : wall_top_y;

        int wall_bottom_y = (WINDOW_HEIGHT / 2) + (wall_height / 2);
        wall_bottom_y =
            wall_bottom_y > WINDOW_HEIGHT ? WINDOW_HEIGHT : wall_bottom_y;

        // Draw the ceiling with a solid color
        for (int y = 0; y < wall_top_y; y++) {
            draw_pixel(x, y, 0xFF333333);
        }

        int texture_offset_x;

        if (rays[x].was_hit_vertical) {
            texture_offset_x = (int)rays[x].wall_hit_y % TILE_SIZE;
        } else {
            texture_offset_x = (int)rays[x].wall_hit_x % TILE_SIZE;
        }

        int tex_num = rays[x].texture - 1;
        int texture_width = upng_get_width(textures[tex_num]);
        int texture_height = upng_get_height(textures[tex_num]);

        // Render the wall with the texture mapping
        for (int y = wall_top_y; y < wall_bottom_y; y++) {
            int distance_from_top = y + (wall_height / 2) - (WINDOW_HEIGHT / 2);

            int texture_offset_y =
                distance_from_top * ((float)texture_height / wall_height);

            color_t* wall_texture_buffer =
                (color_t*)upng_get_buffer(textures[tex_num]);

            color_t texel_color = wall_texture_buffer
                [(texture_width * texture_offset_y) + texture_offset_x];

            // Make the pixels of the wall that were hit vertically darker to create a shadow effect
            if (rays[x].was_hit_vertical) {
                change_color_intensity(&texel_color, 0.7);
            }

            // Set the color of the wall based on the color from the texture
            draw_pixel(x, y, texel_color);
        }

        // Draw the floor with a solid color
        for (int y = wall_bottom_y; y < WINDOW_HEIGHT; y++) {
            draw_pixel(x, y, 0xFF777777);
        }
    }
}
