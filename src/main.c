#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "base_defs.h"
#include "graphics.h"
#include "map.h"
#include "player.h"
#include "ray.h"
#include "textures.h"
#include "wall.h"

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
    return EXIT_SUCCESS;
}

void setup() {
    // Asks uPGN library to decode all PNG files and loads the wall textures array
    load_wall_textures();
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
    render_rays();
    render_player();

    render_color_buffer();
}

void release_resources(void) {
    free_wall_textures();
    destroy_window();
}
