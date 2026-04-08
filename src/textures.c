#include "textures.h"

#include <stdio.h>

static const char* texture_file_names[NUM_TEXTURES] = {
    "./images/redbrick.png",     // [0]
    "./images/purplestone.png",  // [1]
    "./images/mossystone.png",   // [2]
    "./images/graystone.png",    // [3]
    "./images/colorstone.png",   // [4]
    "./images/bluestone.png",    // [5]
    "./images/wood.png",         // [6]
    "./images/eagle.png",        // [7]
    "./images/pikuma.png",       // [8]
    "./images/barrel.png",       // [9]
    "./images/light.png",        // [10]
    "./images/table.png",        // [11]
    "./images/guard.png",        // [12]
    "./images/armor.png"         // [13]
};

void load_textures() {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        upng_t* upng = upng_new_from_file(texture_file_names[i]);

        if (upng == NULL) {
            printf(
                "Error allocating memory for texture: %s\n",
                texture_file_names[i]
            );
            continue;
        }

        upng_decode(upng);
        if (upng_get_error(upng) != UPNG_EOK) {
            printf("Error decoding texture file: %s\n", texture_file_names[i]);
            upng_free(upng);
            continue;
        }

        textures[i] = upng;
    }
}

void free_textures(void) {
    for (int i = 0; i < NUM_TEXTURES; i++) {
        upng_free(textures[i]);
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
