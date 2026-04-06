#ifndef BASE_DEFS_H
#define BASE_DEFS_H

#include <math.h>
#include <stdint.h>

#define PI 3.14159265
#define TWO_PI 6.28318530

#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20
#define NUM_TEXTURES 9

#define MINIMAP_SCALE_FACTOR 0.2

#define WINDOW_WIDTH (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)

#define FOV_ANGLE (60 * (PI / 180))
#define NUM_RAYS WINDOW_WIDTH

#define DIST_PROJ_PLANE ((WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2))

#define FPS 30
#define FRAME_TIME_LENGHT (1000 / FPS)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#endif
