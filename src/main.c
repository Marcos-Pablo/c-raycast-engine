#include "base_defs.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

bool initialize_window();
void destroy_window();
void setup();
void render();
void process_input();
void update();
void render_map();
void render_player();
void move_player(float delta_time);
bool has_wall_at(float x, float y);

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
bool is_game_running = false;
int ticks_last_frame = 0;

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct Player {
    float x;
    float y;
    float width;
    float height;
    int turn_direction; // -1 for left, +1 for right
    int walk_direction; // -1 for back, +1 for front
    float rotation_angle;
    float walk_speed;
    float turn_speed;
} player;

int main() {
    is_game_running = initialize_window();

    setup();
    while (is_game_running) {
        process_input();
        update();
        render();
    }

    destroy_window();
    return 0;
}

bool initialize_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    return true;
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void setup() {
    player.x = WINDOW_WIDTH / 2.0;
    player.y = WINDOW_HEIGHT / 2.0;
    player.width = 1;
    player.height = 1;
    player.turn_direction = 0;
    player.walk_direction = 0;
    player.rotation_angle = PI / 2;
    player.walk_speed = 100;
    player.turn_speed = 45 * (PI / 180);
}

bool has_wall_at(float x, float y) {
    int map_x = floor(x / TILE_SIZE);
    int map_y = floor(y / TILE_SIZE);

    if (map_x < 0 || map_x >= MAP_NUM_COLS) return true;
    if (map_y < 0 || map_y >= MAP_NUM_ROWS) return true;
    return map[map_y][map_x] == 1;
}

void move_player(float delta_time) {
    player.rotation_angle +=
        player.turn_direction * player.turn_speed * delta_time;

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

void render_player() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect player_rect = {
        MINIMAP_SCALE_FACTOR * player.x,
        MINIMAP_SCALE_FACTOR * player.y,
        MINIMAP_SCALE_FACTOR * player.width,
        MINIMAP_SCALE_FACTOR * player.height
    };

    SDL_RenderFillRect(renderer, &player_rect);

    SDL_RenderDrawLine(
        renderer,
        MINIMAP_SCALE_FACTOR * player.x,
        MINIMAP_SCALE_FACTOR * player.y,
        MINIMAP_SCALE_FACTOR * player.x + cos(player.rotation_angle) * 40,
        MINIMAP_SCALE_FACTOR * player.y + sin(player.rotation_angle) * 40
    );
}

void render_map() {
    for (int i = 0; i < MAP_NUM_ROWS; i++) {
        for (int j = 0; j < MAP_NUM_COLS; j++) {
            int tile_x = j * TILE_SIZE;
            int tile_y = i * TILE_SIZE;
            int tile_color = map[i][j] == 1 ? 255 : 0;
            SDL_SetRenderDrawColor(
                renderer,
                tile_color,
                tile_color,
                tile_color,
                255
            );
            SDL_Rect map_tile_rect = {
                MINIMAP_SCALE_FACTOR * tile_x,
                MINIMAP_SCALE_FACTOR * tile_y,
                MINIMAP_SCALE_FACTOR * TILE_SIZE,
                MINIMAP_SCALE_FACTOR * TILE_SIZE
            };
            SDL_RenderFillRect(renderer, &map_tile_rect);
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
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // TODO:
    // Render game objects here

    render_map();
    render_player();

    SDL_RenderPresent(renderer);
}
