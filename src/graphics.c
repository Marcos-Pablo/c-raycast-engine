#include "graphics.h"

#include "base_defs.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *color_buffer_texture;
static u32 *color_buffer = NULL;

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int full_screen_width = display_mode.w;
    int full_screen_height = display_mode.h;

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        full_screen_width,
        full_screen_height,
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

    color_buffer =
        (u32 *)malloc(sizeof(u32) * (u32)WINDOW_WIDTH * (u32)WINDOW_HEIGHT);

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    return true;
}

void destroy_window(void) {
    free(color_buffer);
    SDL_DestroyTexture(color_buffer_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void clear_color_buffer(u32 color) {
    for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
        color_buffer[i] = color;
    }
}

void render_color_buffer(void) {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)((u32)WINDOW_WIDTH * sizeof(u32))
    );

    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void draw_pixel(int x, int y, u32 color) {
    if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT) {
        color_buffer[(y * WINDOW_WIDTH) + x] = color;
    }
}

void draw_rect(int x, int y, int width, int height, u32 color) {
    if (x < 0 || x > WINDOW_WIDTH) return;
    if (y < 0 || y > WINDOW_HEIGHT) return;

    for (int col_offset = 0; col_offset < width; col_offset++) {
        if (x + col_offset > WINDOW_WIDTH) {
            break;
        }

        for (int row_offset = 0; row_offset < height; row_offset++) {
            if (y + row_offset > WINDOW_HEIGHT) {
                break;
            }

            draw_pixel(x + col_offset, y + row_offset, color);
        }
    }
}
