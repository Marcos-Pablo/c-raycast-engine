#include "sprites.h"

#define NUM_SPRITES 6

#include "ray.h"

static sprite_t sprites[NUM_SPRITES] = {
    {.x = 640, .y = 630, .texture = 9},   // barrel
    {.x = 660, .y = 690, .texture = 9},   // barrel
    {.x = 250, .y = 600, .texture = 11},  // table
    {.x = 250, .y = 600, .texture = 10},  // light
    {.x = 300, .y = 400, .texture = 12},  // guard
    {.x = 90, .y = 100, .texture = 13},   // armor
};

void render_map_sprites(void) {
    for (int i = 0; i < NUM_SPRITES; i++) {
        draw_rect(
            MINIMAP_SCALE_FACTOR * sprites[i].x,
            MINIMAP_SCALE_FACTOR * sprites[i].y,
            2,
            2,
            (sprites[i].visible) ? 0xFF00FFFF : 0xFF444444
        );
    }
}

void render_sprite_projection(void) {
    sprite_t visible_sprites[NUM_SPRITES];
    int num_visible_sprites = 0;

    // Find sprites that are visible (inside the FOV)
    for (int i = 0; i < NUM_SPRITES; i++) {
        float angle_sprite_player =
            player.rotation_angle -
            atan2(sprites[i].y - player.y, sprites[i].x - player.x);

        // Make sure the angle is between 0 and PI
        if (angle_sprite_player < -PI) {
            angle_sprite_player += TWO_PI;
        }

        if (angle_sprite_player > PI) {
            angle_sprite_player -= TWO_PI;
        }

        angle_sprite_player = fabs(angle_sprite_player);

        // if sprite angle is less than half the FOV plus a small margin of error
        const float EPSILON = 0.2;
        if (angle_sprite_player < (FOV_ANGLE / 2) + EPSILON) {
            sprites[i].visible = true;
            sprites[i].angle = angle_sprite_player;
            sprites[i].distance = distance_between_points(
                player.x, player.y, sprites[i].x, sprites[i].y
            );
            visible_sprites[num_visible_sprites] = sprites[i];
            num_visible_sprites++;
        } else {
            sprites[i].visible = false;
        }
    }

    if (num_visible_sprites == 0) {
        return;
    }

    // Sort sprites by distance from the player (farthest to closest) using bubble sort
    for (int i = 0; i < num_visible_sprites - 1; i++) {
        for (int j = i + 1; j < num_visible_sprites; j++) {
            if (visible_sprites[i].distance < visible_sprites[j].distance) {
                sprite_t temp = visible_sprites[i];
                visible_sprites[i] = visible_sprites[j];
                visible_sprites[j] = temp;
            }
        }
    }

    for (int i = 0; i < num_visible_sprites; i++) {
        sprite_t sprite = visible_sprites[i];

        // Calculate perpendicular distance to the sprite to avoid fish-eye distortion
        float perp_distance = sprite.distance * cos(sprite.angle);

        // Calculate the sprite projected height and width (assuming the sprite is a square)
        float sprite_height = (TILE_SIZE / perp_distance) * DIST_PROJ_PLANE;
        float sprite_width = sprite_height;

        float sprite_top_y = (WINDOW_HEIGHT / 2) - (sprite_height / 2);
        sprite_top_y = sprite_top_y < 0 ? 0 : sprite_top_y;

        float sprite_bottom_y = (WINDOW_HEIGHT / 2) + (sprite_height / 2);
        sprite_bottom_y =
            sprite_bottom_y > WINDOW_HEIGHT ? WINDOW_HEIGHT : sprite_bottom_y;

        // Calculate the sprite x position in the projection plane
        float sprite_angle = atan2(sprite.y - player.y, sprite.x - player.x) -
                             player.rotation_angle;

        float sprite_screen_x = tan(sprite_angle) * DIST_PROJ_PLANE;

        float sprite_left_x =
            (WINDOW_WIDTH / 2) + sprite_screen_x - (sprite_width / 2);

        float sprite_right_x = sprite_left_x + sprite_width;

        int texture_width = upng_get_width(textures[sprite.texture]);
        int texture_height = upng_get_height(textures[sprite.texture]);

        for (int x = sprite_left_x; x < sprite_right_x; x++) {
            if (x < 0 || x >= WINDOW_WIDTH) continue;

            float texel_width = (texture_width / sprite_width);
            int texture_offset_x = (x - sprite_left_x) * texel_width;

            for (int y = sprite_top_y; y < sprite_bottom_y; y++) {
                if (y < 0 || y >= WINDOW_HEIGHT) continue;

                int distance_from_top =
                    y + (sprite_height / 2) - (WINDOW_HEIGHT / 2);

                int texture_offset_y =
                    distance_from_top * (texture_height / sprite_height);

                color_t* sprite_texture_buffer =
                    (color_t*)upng_get_buffer(textures[sprite.texture]);

                color_t texel_color = sprite_texture_buffer
                    [(texture_offset_y * texture_width) + texture_offset_x];

                if (texel_color == 0xFFFF00FF) continue;
                if (sprite.distance > rays[x].distance) continue;

                draw_pixel(x, y, texel_color);
            }
        }
    }
}
