#include <SDL2/SDL.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "base_defs.h"
#include "graphics.h"
#include "map.h"
#include "player.h"
#include "ray.h"
#include "textures.h"

void setup();
void render();
void process_input();
void update();
void release_resources(void);

bool is_game_running = false;
int ticks_last_frame = 0;

int main() {
    is_game_running = initialize_window();

    setup();
    while (is_game_running) {
        process_input();
        update();
        render();
    }

    release_resources();
    return 0;
}

void setup() {
    // Asks uPGN library to decode all PNG files and loads the wall textures array
    load_wall_textures();
}

void render_wall_projection() {
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

            u32 texel_color =
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

void process_input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            is_game_running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    is_game_running = false;
                    break;

                case SDLK_UP:
                    player.walk_direction = +1;
                    break;

                case SDLK_DOWN:
                    player.walk_direction = -1;
                    break;

                case SDLK_RIGHT:
                    player.turn_direction = +1;
                    break;

                case SDLK_LEFT:
                    player.turn_direction = -1;
                    break;
            }

            break;

        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    player.walk_direction = 0;
                    break;

                case SDLK_DOWN:
                    player.walk_direction = 0;
                    break;

                case SDLK_RIGHT:
                    player.turn_direction = 0;
                    break;

                case SDLK_LEFT:
                    player.turn_direction = 0;
                    break;
            }

            break;
    }
}

void update() {
    // waste some time until we reach the target frame time length
    int time_to_wait = FRAME_TIME_LENGHT - (SDL_GetTicks() - ticks_last_frame);

    if (time_to_wait > 0 && time_to_wait <= FRAME_TIME_LENGHT) {
        SDL_Delay(time_to_wait);
    }

    float delta_time = (SDL_GetTicks() - ticks_last_frame) / 1000.0f;

    ticks_last_frame = SDL_GetTicks();

    move_player(delta_time);
    cast_all_rays();
}

void render() {
    clear_color_buffer(0xFF000000);
    render_wall_projection();

    render_map();
    // render_rays();
    render_player();

    render_color_buffer();
}

void release_resources(void) {
    free_wall_textures();
    destroy_window();
}
