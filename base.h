/*
 * base.h
 *
 * Provides the basic functions and data types used by this game.
 */

#pragma once

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include <SDL3/SDL.h>

/* ----------------------------------------------------------------------------
 * Helper macros
 */
using std::abs;
using std::max;
using std::min;
#ifndef SGN
#define SGN(a)   (((a) < 0) ? -1 : ((a) > 0))
#endif

/* ----------------------------------------------------------------------------
 * Key codes (mapped internally to SDL scancodes)
 */
#define KEY_ESC    1
#define KEY_LEFT   2
#define KEY_RIGHT  3
#define KEY_SPACE  4

extern volatile unsigned char key[256];
extern volatile float delta_time;

/* ----------------------------------------------------------------------------
 * Screen dimensions (fixed for this project)
 */
#define SCREEN_W 640
#define SCREEN_H 480

/* ----------------------------------------------------------------------------
 * Types
 */

using Color = SDL_Color;
using Sprite = SDL_Texture;

/** Raw audio buffer and metadata */
struct Sample {
    SDL_AudioSpec spec;
    unsigned char *buffer = nullptr;
    unsigned int length = 0;

    ~Sample() {
        SDL_free(buffer);
    }
};

/** Color helper */
static inline Color rgb(uint8_t r, uint8_t g, uint8_t b) {
    return { r, g, b, 255 };
}

/** Print error message to stderr */
void print_error(const char *fmt, ...);

/* Drawing functions  */
void draw_text(float x, float y, Color color, const char *fmt, ...);
void draw_rect(float x1, float y1, float x2, float y2, Color color);
void draw_line(float x1, float y1, float x2, float y2, Color color);
void draw_point(float x, float y, Color color);
void draw_sprite(Sprite *spr, float x, float y, float alpha = 1.0f);

/* Audio */
void play_sample(Sample *s, float gain = 1.f, int pan = 128,
                 float frequencyRatio = 1.f, int loop = 0);

/* Main loop */
bool init();
void present();
void process_events();
void shutdown();

/* Resources */
Sample *load_sample(const char *filename);
Sprite *load_sprite(const char *filename);
