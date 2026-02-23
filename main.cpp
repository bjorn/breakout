/**********************************************************************************************
 *  BreakOut variant - using particle system
 *  Version 1.0
 *
 *  Changes:
 *   1.0: Initial version.
 */

#define SDL_MAIN_USE_CALLBACKS

#include <cstdlib>
#include <ctime>

#include <SDL3/SDL_main.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#include "base.h"
#include "data.h"
#include "p_engine.h"
#include "ptypes.h"

//=====   Main program   ====================================================================//

// Global variables
Particle_System p;
Data data;

/* Datafile */
void load_data()
{
    data.BALL01_BMP = load_sprite("data/ball01.bmp");
    data.BLIP1_WAV = load_sample("data/BLIP1.wav");
    data.BONUS01_BMP = load_sprite("data/bonus01.bmp");
    data.BORDER_BMP = load_sprite("data/border.bmp");
    data.BRICK01_BMP = load_sprite("data/brick01.bmp");
    data.BRICK02_BMP = load_sprite("data/brick02.bmp");
    data.BRICK03_BMP = load_sprite("data/brick03.bmp");
    data.BRICK03B_BMP = load_sprite("data/brick03b.bmp");
    data.BRICK04_BMP = load_sprite("data/brick04.bmp");
    data.BRICK05_BMP = load_sprite("data/brick05.bmp");
    data.BRICK06_BMP = load_sprite("data/brick06.bmp");
    data.BRICK07_BMP = load_sprite("data/brick07.bmp");
    data.BRICK08_BMP = load_sprite("data/brick08.bmp");
    data.BRICK09_BMP = load_sprite("data/brick09.bmp");
    data.BRICK10_BMP = load_sprite("data/brick10.bmp");
    data.COIN_BMP = load_sprite("data/coin.bmp");
    data.HIT3_WAV = load_sample("data/HIT3.wav");
    data.PAD01_BMP = load_sprite("data/pad01.bmp");
    data.POP1_WAV = load_sample("data/POP1.wav");
    data.POP2_WAV = load_sample("data/POP2.wav");
    data.POP3_WAV = load_sample("data/POP3.wav");
    data.POP4_WAV = load_sample("data/POP4.wav");
    data.POP5_WAV = load_sample("data/POP5.wav");
    data.STARTUP_WAV = load_sample("data/STARTUP.wav");
    data.TIN_WAV = load_sample("data/TIN.wav");
}

SDL_AppResult SDL_AppInit(void ** /*appstate*/, int /*argc*/, char ** /*argv*/)
{
    if (!init()) {
        print_error("Failed to initialize SDL (%s)", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    load_data();

    // Basic nullptr asset checks for critical sprites
    if (!data.BORDER_BMP || !data.PAD01_BMP || !data.BALL01_BMP) {
        print_error("Critical assets failed to load.");
        return SDL_APP_FAILURE;
    }

    std::srand(std::time(nullptr));

    // Add initial particles to the particle system
    p.add_particle(new BreakoutGame);
    p.add_particle(new StarField);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void * /*appstate*/, SDL_Event *event)
{
    if (handle_event(*event)) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void * /*appstate*/)
{
#ifdef __EMSCRIPTEN__
    static bool forcedRafTiming = false;
    if (!forcedRafTiming) {
        // SDL callback setup may override timing after init; force RAF once here.
        emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
        forcedRafTiming = true;
    }
#endif

    update_input_state();
    p.update_particles(delta_time);
    p.draw_particles();
    present();

    if (key[KEY_ESC]) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void * /*appstate*/, SDL_AppResult /*result*/)
{
    p.remove_particles();
    shutdown();
}
