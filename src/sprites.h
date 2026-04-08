#ifndef SPRITES_H
#define SPRITES_H

#include <stdbool.h>

#include "base_defs.h"
#include "graphics.h"
#include "player.h"
#include "textures.h"
#include "utils.h"

typedef struct {
    float x;
    float y;
    float distance;
    float angle;
    int texture;
    bool visible;
} sprite_t;

void render_sprite_projection(void);
void render_map_sprites(void);

#endif
