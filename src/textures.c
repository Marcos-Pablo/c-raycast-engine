#include "textures.h"

#include <stdio.h>

static const char* texture_file_names[NUM_TEXTURES] = {
    "./images/redbrick.png",
    "./images/purplestone.png",
    "./images/mossystone.png",
    "./images/graystone.png",
    "./images/colorstone.png",
    "./images/bluestone.png",
    "./images/wood.png",
    "./images/eagle.png",
    "./images/pikuma.png"
};

void load_wall_textures() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        upng_t* upng;

        upng = upng_new_from_file(texture_file_names[i]);

        if (upng != NULL) {
            upng_decode(upng);
            if (upng_get_error(upng) == UPNG_EOK) {
                wall_textures[i].upng_texture = upng;
                wall_textures[i].width = upng_get_width(upng);
                wall_textures[i].height = upng_get_height(upng);
                wall_textures[i].texture_buffer =
                    (color_t*)upng_get_buffer(upng);
            }
        }
    }
}

void free_wall_textures() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        upng_free(wall_textures[i].upng_texture);
    }
}

/////////////////////////////////////////////////////////////////////////
// Change the color intensity based on the factor value between 0 and 1
/////////////////////////////////////////////////////////////////////////
void change_color_intensity(color_t* color, float factor) {
    color_t a = (*color & 0xFF000000);
    color_t r = (*color & 0x00FF0000) * factor;
    color_t g = (*color & 0x0000FF00) * factor;
    color_t b = (*color & 0x000000FF) * factor;

    *color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
}
