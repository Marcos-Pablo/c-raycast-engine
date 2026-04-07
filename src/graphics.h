#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "base_defs.h"

bool initialize_window(void);
void destroy_window(void);
void clear_color_buffer(color_t color);
void render_color_buffer(void);
void draw_pixel(int x, int y, color_t color);
void draw_rect(int x, int y, int width, int height, color_t color);
void draw_line(int x1, int y1, int x2, int y2, color_t color);

#endif
