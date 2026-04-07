#ifndef TEXTURES_H
#define TEXTURES_H

#include "base_defs.h"
#include "upng.h"

typedef struct {
    upng_t* upng_texture;
    int width;
    int height;
    color_t* texture_buffer;
} texture_t;

texture_t wall_textures[NUM_TEXTURES];

void load_wall_textures();
void free_wall_textures();

#endif
