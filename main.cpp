/**********************************************************************************************
 *  BreakOut variant - using particle system
 *  Version 1.0
 *
 *  Changes:
 *   1.0: Initial version.
 */


#include <ctime>
#include <cstdlib>
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
    data.BALL01_BMP = load_sprite("ball01.bmp");
    data.BLIP1_WAV = load_sample("BLIP1.wav");
    data.BONUS01_BMP = load_sprite("bonus01.bmp");
    data.BORDER_BMP = load_sprite("border.bmp");
    data.BRICK01_BMP = load_sprite("brick01.bmp");
    data.BRICK02_BMP = load_sprite("brick02.bmp");
    data.BRICK03_BMP = load_sprite("brick03.bmp");
    data.BRICK03B_BMP = load_sprite("brick03b.bmp");
    data.BRICK04_BMP = load_sprite("brick04.bmp");
    data.BRICK05_BMP = load_sprite("brick05.bmp");
    data.BRICK06_BMP = load_sprite("brick06.bmp");
    data.BRICK07_BMP = load_sprite("brick07.bmp");
    data.BRICK08_BMP = load_sprite("brick08.bmp");
    data.BRICK09_BMP = load_sprite("brick09.bmp");
    data.BRICK10_BMP = load_sprite("brick10.bmp");
    data.COIN_BMP = load_sprite("coin.bmp");
    data.HIT3_WAV = load_sample("HIT3.wav");
    data.PAD01_BMP = load_sprite("pad01.bmp");
    data.POP1_WAV = load_sample("POP1.wav");
    data.POP2_WAV = load_sample("POP2.wav");
    data.POP3_WAV = load_sample("POP3.wav");
    data.POP4_WAV = load_sample("POP4.wav");
    data.POP5_WAV = load_sample("POP5.wav");
    data.STARTUP_WAV = load_sample("STARTUP.wav");
    data.TIN_WAV = load_sample("TIN.wav");
}

int main(int argc, char *argv[])
{
	if (!init()) {
		print_error("Failed to initialize SDL (%s)", SDL_GetError());
		return 1;
	}

	load_data();

	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// Add initial particles to the particle system
	p.add_particle(new BreakoutGame);
	p.add_particle(new StarField);

	// Main loop
	do {
		process_events();
		p.update_particles(delta_time);
		p.draw_particles();
        present();
	} while (!key[KEY_ESC]); // while (running)

	p.remove_particles();

	shutdown();
	return 0;
}
