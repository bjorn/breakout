/*
 * base.h
 *
 * Provides the basic functions and data types used by this game.
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

#include <SDL3/SDL.h>

/* ----------------------------------------------------------------------------
 * Helper functions and macros
 */
using std::abs;
using std::atan2;
using std::clamp;
using std::cos;
using std::max;
using std::min;
using std::sin;
using std::sqrt;
#ifndef SGN
#define SGN(a) (((a) < 0) ? -1 : ((a) > 0))
#endif

/* ----------------------------------------------------------------------------
 * Key codes (mapped internally to SDL scancodes)
 */
inline constexpr int KEY_QUIT = 1;
inline constexpr int KEY_LEFT = 2;
inline constexpr int KEY_RIGHT = 3;
inline constexpr int KEY_ACTION = 4;

extern volatile unsigned char key[256];
extern volatile float delta_time;

/* ----------------------------------------------------------------------------
 * Screen dimensions (fixed for this project)
 */
inline constexpr int SCREEN_W = 640;
inline constexpr int SCREEN_H = 480;

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

    ~Sample() { SDL_free(buffer); }
};

/** Color helper */
[[nodiscard]] inline constexpr Color rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return { r, g, b, 255 };
}
[[nodiscard]] inline constexpr Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return { r, g, b, a };
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
void play_sample(Sample *s, float gain = 1.f, int pan = 128, float frequencyRatio = 1.f, int loop = 0);

/* Main loop */
[[nodiscard]] bool init();
void present();
[[nodiscard]] bool handle_event(const SDL_Event &event);
void update_input_state();
[[nodiscard]] float get_gamepad_left_x();
void rumble_gamepad(Uint16 low_frequency_rumble, Uint16 high_frequency_rumble, Uint32 duration_ms);
void shutdown();

/* Resources */
[[nodiscard]] Sample *load_sample(const char *filename);
[[nodiscard]] Sprite *load_sprite(const char *filename);
