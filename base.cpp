/*
 * base.cpp
 *
 * SDL3-based implementation of the basic functions used by this game.
 */

#include "base.h"

#include <algorithm>
#include <iostream>
#include <vector>

// ----------------------------------------------------------------------------
// Global variables (declared extern in header)
// ----------------------------------------------------------------------------
volatile unsigned char key[256] = {0};
volatile float delta_time = 0.f;

// ----------------------------------------------------------------------------
// Internal SDL state
// ----------------------------------------------------------------------------
static SDL_Window *gWindow = nullptr;
static SDL_Renderer *gRenderer = nullptr;


// ----------------------------------------------------------------------------
// Audio state
// ----------------------------------------------------------------------------
static SDL_AudioDeviceID gAudioDevice = 0;
static constexpr int NUM_AUDIO_STREAMS = 4;
static SDL_AudioStream *gAudioStreams[NUM_AUDIO_STREAMS] = {nullptr, nullptr, nullptr, nullptr};
static int gNextAudioStreamIndex = 0;
static uint64_t gLastTicks = 0;
static const bool *gKeyStates = nullptr;

//=====   Frames per second counter   =======================================================//

volatile unsigned int fps, fps_counter;
Uint32 reset_fps_counter(void *userdata, SDL_TimerID timerID, Uint32 interval)
{
    fps = fps_counter;
    fps_counter = 0;
    return interval;
}

// ----------------------------------------------------------------------------
// Utility
// ----------------------------------------------------------------------------
void print_error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fputc('\n', stderr);
    va_end(ap);
}

// ----------------------------------------------------------------------------
// Drawing functions
// ----------------------------------------------------------------------------
void draw_text(float x, float y, Color color, const char *fmt, ...)
{
    if (!gRenderer || !fmt) return;

    char buffer[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

    SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderDebugText(gRenderer, x, y, buffer);
}

void draw_rect(float x1, float y1, float x2, float y2, Color color)
{
    SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
    SDL_FRect r {
        x1,
        y1,
        x2 - x1 + 1,
        y2 - y1 + 1
    };
    SDL_RenderRect(gRenderer, &r);
}

void draw_line(float x1, float y1, float x2, float y2, Color color)
{
    SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderLine(gRenderer, x1, y1, x2, y2);
}

void draw_point(float x, float y, Color color)
{
    SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);
    SDL_RenderPoint(gRenderer, x, y);
}

void draw_sprite(Sprite *spr, float x, float y, float alpha)
{
    if (!spr || !spr)
        return;

    const SDL_FRect r = {
        x,
        y,
        static_cast<float>(spr->w),
        static_cast<float>(spr->h)
    };
    SDL_SetTextureAlphaModFloat(spr, alpha);
    SDL_RenderTexture(gRenderer, spr, nullptr, &r);
}

// ----------------------------------------------------------------------------
// Audio playback
// ----------------------------------------------------------------------------
static inline SDL_AudioStream *next_audio_stream()
{
    SDL_AudioStream *stream = gAudioStreams[gNextAudioStreamIndex];
    gNextAudioStreamIndex = (gNextAudioStreamIndex + 1) % NUM_AUDIO_STREAMS;
    return stream;
}

void play_sample(Sample *s, float gain, int pan, float frequencyRatio, int loop)
{
    (void)pan;  // panning would require processing the sample's buffer
    (void)loop; // looping would require additional bookkeeping

    if (!s || !s->buffer || !s->length)
        return;

    SDL_AudioStream *stream = next_audio_stream();
    if (!stream)
        return;

    if (!SDL_SetAudioStreamFormat(stream, &s->spec, nullptr)) {
        print_error("Warning: SDL_SetAudioStreamFormat failed (%s)", SDL_GetError());
        return;
    }

    // Replace any queued sample and set the gain and frequency ratio
    SDL_ClearAudioStream(stream);
    SDL_SetAudioStreamGain(stream, gain);
    SDL_SetAudioStreamFrequencyRatio(stream,
                                     std::clamp(frequencyRatio, 0.01f, 100.f));

    if (!SDL_PutAudioStreamData(stream, s->buffer, s->length)) {
        print_error("Warning: SDL_PutAudioStreamData failed (%s)", SDL_GetError());
    }
}

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------
bool init()
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        print_error("Warning: Failed to initialize SDL (%s)", SDL_GetError());
        return false;
    }

    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE;

    if (!SDL_CreateWindowAndRenderer("Breakout", SCREEN_W, SCREEN_H, window_flags, &gWindow, &gRenderer)) {
        print_error("Warning: Failed to create window and renderer (%s)", SDL_GetError());
        return false;
    }

    SDL_SetRenderVSync(gRenderer, 1);
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderLogicalPresentation(gRenderer, SCREEN_W, SCREEN_H, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_AddTimer(1000, reset_fps_counter, nullptr);

    std::fill(std::begin(key), std::end(key), 0);

    gKeyStates = SDL_GetKeyboardState(nullptr);

    // Create multiple audio streams for polyphonic playback (formats are set later)
    for (int i = 0; i < NUM_AUDIO_STREAMS; ++i) {
        gAudioStreams[i] = SDL_CreateAudioStream(nullptr, nullptr);
        if (!gAudioStreams[i])
            print_error("Warning: Failed to create audio stream %d (%s)", i, SDL_GetError());
    }

    // Open the default playback device
    gAudioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!gAudioDevice)
        print_error("Warning: Failed to open audio device (%s)", SDL_GetError());
    else if (!SDL_BindAudioStreams(gAudioDevice, gAudioStreams, NUM_AUDIO_STREAMS))
        print_error("Warning: Failed to bind audio streams (%s)", SDL_GetError());

    return true;
}

// ----------------------------------------------------------------------------
// Present
// ----------------------------------------------------------------------------
void present()
{
    draw_text(0, 0, rgb(100,100,100), "%d fps", fps);
    fps_counter++;

    SDL_RenderPresent(gRenderer);

    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(gRenderer);

    // Update delta time
    const auto lastTicks = gLastTicks;
    gLastTicks = SDL_GetTicks();
    const auto deltaTicks = static_cast<int>(gLastTicks - lastTicks);
    delta_time = std::clamp(deltaTicks, 0, 1000) / 1000.f;
}

// ----------------------------------------------------------------------------
// Event processing / input mapping
// ----------------------------------------------------------------------------
void process_events()
{
    SDL_Event e;
    bool quit = false;

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT) {
            quit = true;
        } else if (e.type == SDL_EVENT_KEY_DOWN) {
            switch (e.key.key) {
                case SDLK_F: {
                    // toggle fullscreen
                    bool fullscreen = SDL_GetWindowFlags(gWindow) & SDL_WINDOW_FULLSCREEN;
                    SDL_SetWindowFullscreen(gWindow, fullscreen ? 0 : SDL_WINDOW_FULLSCREEN);
                    break;
                }
                case SDLK_V: {
                    // toggle vsync
                    int vsync;
                    if (SDL_GetRenderVSync(gRenderer, &vsync)) {
                        SDL_SetRenderVSync(gRenderer, vsync ? 0 : 1);
                        std::cout << "VSync " << (vsync ? "off" : "on") << std::endl;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    key[KEY_ESC] = gKeyStates[SDL_SCANCODE_ESCAPE] || quit;
    key[KEY_LEFT] = gKeyStates[SDL_SCANCODE_LEFT];
    key[KEY_RIGHT] = gKeyStates[SDL_SCANCODE_RIGHT];
    key[KEY_SPACE] = gKeyStates[SDL_SCANCODE_SPACE];
}

// ----------------------------------------------------------------------------
// Shutdown
// ----------------------------------------------------------------------------
void shutdown()
{
    // Destroy audio streams first.
    for (int i = 0; i < NUM_AUDIO_STREAMS; ++i) {
        if (gAudioStreams[i]) {
            SDL_DestroyAudioStream(gAudioStreams[i]);
            gAudioStreams[i] = nullptr;
        }
    }

    // Close audio device.
    if (gAudioDevice) {
        SDL_CloseAudioDevice(gAudioDevice);
        gAudioDevice = 0;
    }

    if (gRenderer) {
        SDL_DestroyRenderer(gRenderer);
        gRenderer = nullptr;
    }

    if (gWindow) {
        SDL_DestroyWindow(gWindow);
        gWindow = nullptr;
    }

    SDL_Quit();
}

// ----------------------------------------------------------------------------
// Resources
// ----------------------------------------------------------------------------
Sample *load_sample(const char *filename)
{
    if (!filename) return nullptr;

    Sample *sample = new Sample;

    if (!SDL_LoadWAV(filename, &sample->spec, &sample->buffer, &sample->length)) {
        print_error("Failed loading WAV: %s (%s)", filename, SDL_GetError());
        delete sample;
        return nullptr;
    }

    return sample;
}

Sprite *load_sprite(const char *filename)
{
    SDL_Surface *surf = SDL_LoadBMP(filename);
    if (!surf) {
        print_error("Failed loading BMP: %s (%s)", filename, SDL_GetError());
        return nullptr;
    }

    // Set magic pink (RGB 255,0,255) as transparent via color key
    const SDL_PixelFormatDetails *fmtDetails = SDL_GetPixelFormatDetails(surf->format);
    if (!fmtDetails) {
        print_error("Warning: SDL_GetPixelFormatDetails failed (%s)", SDL_GetError());
    } else {
        Uint32 colorkey = SDL_MapRGB(fmtDetails, nullptr, 255, 0, 255);
        if (!SDL_SetSurfaceColorKey(surf, true, colorkey)) {
            print_error("Warning: Failed to set color key on surface (%s)", SDL_GetError());
        }
    }

    auto *sprite = SDL_CreateTextureFromSurface(gRenderer, surf);
    if (!sprite)
        print_error("Warning: Failed to create texture from surface (%s)", SDL_GetError());

    SDL_DestroySurface(surf);

    return sprite;
}
