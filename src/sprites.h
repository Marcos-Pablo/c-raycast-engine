#ifndef SPRITES_H
#define SPRITES_H

typedef struct {
    float x;
    float y;
    float distance;
    float angle;
    int texture;
} sprite_t;

void render_sprite_projection(void);
void render_map_sprites(void);

#endif
