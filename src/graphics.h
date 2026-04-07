#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "base_defs.h"

bool initialize_window(void);
void destroy_window(void);
void clear_color_buffer(u32 color);
void render_color_buffer(void);
void draw_pixel(int x, int y, u32 color);
void draw_rect(int x, int y, int width, int height, u32 color);

#endif
