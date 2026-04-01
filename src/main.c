#include <SDL2/SDL.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "base_defs.h"
#include "textures.h"

bool initialize_window();
void destroy_window();
void setup();
void render();
void process_input();
void update();

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
bool is_game_running = false;
int ticks_last_frame = 0;

SDL_Texture *color_buffer_texture;
u32 *color_buffer = NULL;

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
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

struct Player {
    float x;
    float y;
    float width;
    float height;
    float rotation_angle;
    float walk_speed;
    float turn_speed;
    int turn_direction;  // -1 for left, +1 for right
    int walk_direction;  // -1 for back, +1 for front
} player;

struct Ray {
    float ray_angle;
    float wall_hit_x;
    float wall_hit_y;
    float distance;
    bool was_hit_vertical;
    bool is_ray_facing_up;
    bool is_ray_facing_down;
    bool is_ray_facing_left;
    bool is_ray_facing_right;
    int wall_hit_content;
} rays[NUM_RAYS];

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
    free(color_buffer);
    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free_wall_textures();
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

    color_buffer =
        (u32 *)malloc(sizeof(u32) * (u32)WINDOW_WIDTH * (u32)WINDOW_HEIGHT);

    // Asks uPGN library to decode all PNG files and loads the wall textures array
    load_wall_textures();

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );
}

bool has_wall_at(float x, float y) {
    int map_x = floor(x / TILE_SIZE);
    int map_y = floor(y / TILE_SIZE);

    if (map_x < 0 || map_x >= MAP_NUM_COLS) return true;
    if (map_y < 0 || map_y >= MAP_NUM_ROWS) return true;
    return map[map_y][map_x] != 0;
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

float normalize_angle(float angle) {
    angle = remainder(angle, TWO_PI);

    if (angle < 0) {
        angle += TWO_PI;
    }

    return angle;
}

float distance_between_points(float x1, float y1, float x2, float y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void cast_ray(int strip_id, float ray_angle) {
    ray_angle = normalize_angle(ray_angle);

    bool is_ray_facing_down = ray_angle > 0 && ray_angle < PI;
    bool is_ray_facing_up = !is_ray_facing_down;

    bool is_ray_facing_right = ray_angle < 0.5 * PI || ray_angle > 1.5 * PI;
    bool is_ray_facing_left = !is_ray_facing_right;

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
    yintercept += is_ray_facing_down ? TILE_SIZE : 0;

    // Find the x-coordinate of the closest horizontal grid intersetion
    xintercept = player.x + (yintercept - player.y) / tan(ray_angle);

    // Calculate the increment xstep and ystep
    ystep = TILE_SIZE;
    ystep *= is_ray_facing_up ? -1 : 1;

    xstep = TILE_SIZE / tan(ray_angle);
    xstep *= (is_ray_facing_left && xstep > 0) ? -1 : 1;
    xstep *= (is_ray_facing_right && xstep < 0) ? -1 : 1;

    float next_horz_touch_x = xintercept;
    float next_horz_touch_y = yintercept;

    // Increment x_step and y_step until we find a wall
    while (next_horz_touch_x >= 0 && next_horz_touch_x <= WINDOW_WIDTH &&
           next_horz_touch_y >= 0 && next_horz_touch_y <= WINDOW_HEIGHT) {
        float x_to_check = next_horz_touch_x;
        float y_to_check = next_horz_touch_y + (is_ray_facing_up ? -1 : 0);

        if (has_wall_at(x_to_check, y_to_check)) {
            found_horz_wall_hit = true;
            horz_wall_hit_x = next_horz_touch_x;
            horz_wall_hit_y = next_horz_touch_y;
            horz_wall_content = map[(int)floor(y_to_check / TILE_SIZE)]
                                   [(int)floor(x_to_check / TILE_SIZE)];
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
    xintercept += is_ray_facing_right ? TILE_SIZE : 0;

    // Find the y-coordinate of the closest vertical grid intersetion
    yintercept = player.y + (xintercept - player.x) * tan(ray_angle);

    // Calculate the increment xstep and ystep
    xstep = TILE_SIZE;
    xstep *= is_ray_facing_left ? -1 : 1;

    ystep = TILE_SIZE * tan(ray_angle);
    ystep *= (is_ray_facing_up && ystep > 0) ? -1 : 1;
    ystep *= (is_ray_facing_down && ystep < 0) ? -1 : 1;

    float next_vert_touch_x = xintercept;
    float next_vert_touch_y = yintercept;

    // Increment x_step and y_step until we find a wall
    while (next_vert_touch_x >= 0 && next_vert_touch_x <= WINDOW_WIDTH &&
           next_vert_touch_y >= 0 && next_vert_touch_y <= WINDOW_HEIGHT) {
        float x_to_check = next_vert_touch_x + (is_ray_facing_left ? -1 : 0);
        float y_to_check = next_vert_touch_y;

        if (has_wall_at(x_to_check, y_to_check)) {
            found_vert_wall_hit = true;
            vert_wall_hit_x = next_vert_touch_x;
            vert_wall_hit_y = next_vert_touch_y;
            vert_wall_content = map[(int)floor(y_to_check / TILE_SIZE)]
                                   [(int)floor(x_to_check / TILE_SIZE)];
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
    rays[strip_id].is_ray_facing_down = is_ray_facing_down;
    rays[strip_id].is_ray_facing_up = is_ray_facing_up;
    rays[strip_id].is_ray_facing_left = is_ray_facing_left;
    rays[strip_id].is_ray_facing_right = is_ray_facing_right;
}

void cast_all_rays() {
    float ray_angle = player.rotation_angle - (FOV_ANGLE / 2);

    for (int strip_id = 0; strip_id < NUM_RAYS; strip_id++) {
        cast_ray(strip_id, ray_angle);
        ray_angle += FOV_ANGLE / NUM_RAYS;
    }
}

void render_map() {
    for (int i = 0; i < MAP_NUM_ROWS; i++) {
        for (int j = 0; j < MAP_NUM_COLS; j++) {
            int tile_x = j * TILE_SIZE;
            int tile_y = i * TILE_SIZE;
            int tile_color = map[i][j] != 0 ? 255 : 0;

            SDL_SetRenderDrawColor(
                renderer, tile_color, tile_color, tile_color, 255
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

void render_rays() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < NUM_RAYS; i++) {
        SDL_RenderDrawLine(
            renderer,
            MINIMAP_SCALE_FACTOR * player.x,
            MINIMAP_SCALE_FACTOR * player.y,
            MINIMAP_SCALE_FACTOR * rays[i].wall_hit_x,
            MINIMAP_SCALE_FACTOR * rays[i].wall_hit_y
        );
    }
}

void render_color_buffer() {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)((u32)WINDOW_WIDTH * sizeof(u32))
    );

    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(u32 color) {
    for (int x = 0; x < WINDOW_WIDTH; x++) {
        for (int y = 0; y < WINDOW_HEIGHT; y++) {
            int idx = (y * WINDOW_WIDTH) + x;

            color_buffer[idx] = color;
        }
    }
}

void generate_3D_projection() {
    for (int i = 0; i < NUM_RAYS; i++) {
        float perpendicular_distance =
            rays[i].distance * cos(rays[i].ray_angle - player.rotation_angle);

        float dist_proj_plane = ((float)WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);

        float projected_wall_height =
            (TILE_SIZE / perpendicular_distance) * dist_proj_plane;

        int wall_strip_height = (int)projected_wall_height;
        int wall_top_pixel = (WINDOW_HEIGHT / 2) - (wall_strip_height / 2);
        wall_top_pixel = wall_top_pixel < 0 ? 0 : wall_top_pixel;

        int wall_bottom_pixel = (WINDOW_HEIGHT / 2) + (wall_strip_height / 2);
        wall_bottom_pixel = wall_bottom_pixel > WINDOW_HEIGHT
                                ? WINDOW_HEIGHT
                                : wall_bottom_pixel;

        for (int y = 0; y < wall_top_pixel; y++) {
            int idx = (y * WINDOW_WIDTH) + i;
            color_buffer[idx] = 0xFF333333;
        }

        int texture_offset_x;

        if (rays[i].was_hit_vertical) {
            texture_offset_x = (int)rays[i].wall_hit_y % TILE_SIZE;
        } else {
            texture_offset_x = (int)rays[i].wall_hit_x % TILE_SIZE;
        }

        int tex_num = rays[i].wall_hit_content - 1;

        for (int y = wall_top_pixel; y < wall_bottom_pixel; y++) {
            int distance_from_top =
                y + (wall_strip_height / 2) - (WINDOW_HEIGHT / 2);

            int texture_offset_y =
                distance_from_top * ((float)TEXTURE_HEIGHT / wall_strip_height);

            u32 texel_color =
                wall_textures[tex_num].texture_buffer
                    [(TEXTURE_WIDTH * texture_offset_y) + texture_offset_x];

            // Set the color of the wall based on the color from the texture
            int idx = (y * WINDOW_WIDTH) + i;
            color_buffer[idx] = texel_color;
        }

        for (int y = wall_bottom_pixel; y < WINDOW_HEIGHT; y++) {
            int idx = (y * WINDOW_WIDTH) + i;
            color_buffer[idx] = 0xFF777777;
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    generate_3D_projection();

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    // display the minimap
    render_map();
    render_rays();
    render_player();

    SDL_RenderPresent(renderer);
}
