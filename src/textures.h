#ifndef TEXTURES_H
#define TEXTURES_H

#include "base_defs.h"
#include "upng.h"

#define NUM_TEXTURES 14

upng_t* textures[NUM_TEXTURES];

void load_textures(void);
void free_textures(void);

void change_color_intensity(color_t* color, float factor);

#endif
