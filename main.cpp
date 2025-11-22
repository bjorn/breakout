/**********************************************************************************************
 *  BreakOut variant - using particle system
 *  Version 1.0
 *
 *  Changes:
 *   1.0: Initial version.
 */


#include <time.h>
#include <allegro.h>

#include "p_engine.h"
#include "ptypes.h"



//=====   Frames per second counter   =======================================================//


volatile unsigned int  fps, fps_counter;
volatile unsigned long ticks_counter;
void reset_fps_counter() {fps = fps_counter; fps_counter = 0;} END_OF_FUNCTION(reset_fps_counter);
void count_tick() {ticks_counter++;} END_OF_FUNCTION(count_tick);



//=====   Main program   ====================================================================//

// Function headers
int  initialize_display();
void update_display();
void exit_display();

// Global variables
int display_mode = 2;		// 0=Memory buffer  1=Page flipping  2=Tripple buffering
bool running = true;
Particle_System p;
BITMAP *page[3];
int curr_page = 0;
float time_passed;
DATAFILE *datafile;


int main(int argc, char *argv[])
{
	allegro_init();
	install_keyboard();

	// Check command line arguments
	if (argc == 2) display_mode = atoi(argv[1]);
	else display_mode = 2;

	if (display_mode < 1 || display_mode > 3) {
		allegro_message("\nUsage: 'breakout [display_mode]', where [display_mode] is one of:\n\n"
			"1 = Update screen using memory buffer\n"
			"2 = Update screen using page flipping\n"
			"3 = Update screen using triple buffering\n");
		return 1;
	}

	if (initialize_display()) return 1;
	install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);

	text_mode(0);
	srand((int)time(NULL));

	// Initialize frames per second counter
	LOCK_VARIABLE(fps);
	LOCK_VARIABLE(fps_counter);
	LOCK_VARIABLE(ticks_counter);
	LOCK_FUNCTION(reset_fps_counter);
	LOCK_FUNCTION(count_tick);
	install_int_ex(reset_fps_counter, BPS_TO_TIMER(1));
	install_int_ex(count_tick, 1);

	// Add initial particles to the particle system
	p.add_particle(new BreakoutGame);
	p.add_particle(new StarField);

	// Main loop
	do {
		time_passed = float(ticks_counter) / float(BPS_TO_TIMER(1)); ticks_counter = 0;
		p.update_particles(MIN(time_passed, 0.04));
		update_display();
	} while (!key[KEY_ESC]); // while (running)

	// Delete particles
	p.remove_particles();

	exit_display();
	return 0;
}
END_OF_MAIN();



int initialize_display()
{
	int width = 640, height = 480;
	int driver = GFX_AUTODETECT;

	// Set screen resolution
	set_color_depth(16);
	if (set_gfx_mode(driver, width, height, 0, 0) < 0) {
		set_color_depth(15);
		if (set_gfx_mode(driver, width, height, 0, 0) < 0) {
			allegro_message(allegro_error);
			return 1;
		}
	}

	// Initialize update method
	if (display_mode == 2 && !(gfx_capabilities & GFX_CAN_TRIPLE_BUFFER)) display_mode = 1;

	switch (display_mode) {
	case 2: /* PAGE FLIPPING */
		page[0] = create_video_bitmap(SCREEN_W, SCREEN_H);
		page[1] = create_video_bitmap(SCREEN_W, SCREEN_H);
		if (!(page[0] && page[1])) {
			set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
			allegro_message("Unable to create two %dx%d video memory pages.\n", SCREEN_W, SCREEN_H);
			return 1;
		}
		break;
	case 3: /* TRIPPLE BUFFERING */
		page[0] = create_video_bitmap(SCREEN_W, SCREEN_H);
		page[1] = create_video_bitmap(SCREEN_W, SCREEN_H);
		page[2] = create_video_bitmap(SCREEN_W, SCREEN_H);
		if (!(page[0] && page[1] && page[2])) {
			set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
			allegro_message("Unable to create three %dx%d video memory pages.\n", SCREEN_W, SCREEN_H);
			return 1;
		}
		break;
	default: /* DOUBLE BUFFER */
		page[0] = create_bitmap(SCREEN_W, SCREEN_H);
		if (!page[0]) {
			set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
			allegro_message("Unable to create one %dx%d memory bitmap.\n", SCREEN_W, SCREEN_H);
			return 1;
		}
		break;
	}

	// Load the datafile
	datafile = load_datafile("breakout.dat");
	if (!datafile) {
		set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
		allegro_message("Unable to load datafile: breakout.dat!");
		return 1;
	}

	
	return 0;
}


void update_display()
{
	if (display_mode != 0) acquire_bitmap(page[curr_page]);

	clear_to_color(page[curr_page], makecol(0,0,0));	// Clear bitmap
	p.draw_particles(page[curr_page]);					// Draw particles

	//textprintf(page[curr_page], font, 0, 0, makecol(100,100,100), "%d fps", fps);

	if (display_mode != 0) release_bitmap(page[curr_page]);

	switch (display_mode) {
	case 2:
		// Flip video bitmaps
		show_video_bitmap(page[curr_page]);
		curr_page = 1 - curr_page;
		break;
	case 3:
		// Make sure that the last requested page is displayed on the screen
		do {;} while (poll_scroll());

		// Request the page to be displayed the screen
		request_video_bitmap(page[curr_page]);
		curr_page = (curr_page + 1) % 3;
		break;
	default:
		draw_sprite(screen, page[curr_page], 0, 0);
		break;
	}

	fps_counter++;
}


void exit_display()
{
	// Free page memory

	switch (display_mode) {
	case 2:
		destroy_bitmap(page[0]);
		destroy_bitmap(page[1]);
		break;
	case 3:
		destroy_bitmap(page[0]);
		destroy_bitmap(page[1]);
		destroy_bitmap(page[2]);
		break;
	default:
		destroy_bitmap(page[0]);
		break;
	}
}
