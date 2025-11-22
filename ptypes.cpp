/**********************************************************************************************
 *
 *  Particles used for the Breakout variant, by Bjørn Lindeijer
 *
 */



#include <math.h>
#include <allegro.h>
#include "p_engine.h"
#include "datafile.h"
#include "particle_list.h"
#include "ptypes.h"

extern DATAFILE *datafile;



//=====   BreakoutLevel   ===================================================================//

BreakoutLevel::BreakoutLevel(BreakoutGame *imy_game, int level_nr):
nr_of_bricks(0), nr_of_balls(0),
my_game(imy_game)
{
	int brick[14][20];
	PACKFILE *file = NULL;

	switch(level_nr) {
	case 1: file = pack_fopen("level01.lev", F_READ_PACKED); break;
	case 2: file = pack_fopen("level02.lev", F_READ_PACKED); break;
	case 3: file = pack_fopen("goldrush.lev", F_READ_PACKED); break;
	}

	if (file) {
		pack_fread(brick, sizeof(brick), file);
		pack_fclose(file);
		for (int x = 0; x < 14; x++) {
			for (int y = 0; y < 20; y++) {
				if (brick[x][y] > 0)
					level.add_particle(new Brick(this, 44 + (16 + 32 * x), 39 + (8 + 16 * y), brick[x][y]));
			}
		}
	}

	Ball *first_ball = new Ball(this, SCREEN_W / 2, 0, 0, 0);
	pad = new Pad((38 + 495) / 2, SCREEN_H - 24);
	level.add_particle(first_ball);
	level.add_particle(pad);
	pad->attach_ball(first_ball);

	// Level borders
	level.add_particle(new Block(0, 0, 38, SCREEN_H - 1));				// Left
	level.add_particle(new Block(495, 0, SCREEN_W - 1, SCREEN_H - 1));	// Right
	level.add_particle(new Block(38, 0, 495, 36));						// Top
}

void BreakoutLevel::initialize()
{
	play_sample((SAMPLE*)(datafile[STARTUP_WAV]).dat, 255, 128, 1000, 0);
}

void BreakoutLevel::update(float dt)
{
	level.update_particles(dt);
	if (nr_of_bricks == 0) my_game->level_finished = true;
	if (nr_of_balls == 0 && my_game->balls_left > -1) {
		my_game->balls_left--;
		if (my_game->balls_left >= 0) {
			Ball *new_ball = new Ball(this, SCREEN_W / 2, 0, 0, 0);
			level.add_particle(new_ball);
			pad->attach_ball(new_ball);
		}
	}
}

void BreakoutLevel::draw(BITMAP *bitmap)
{
	level.draw_particles(bitmap);
}

void BreakoutLevel::remove() {level.remove_particles();}

void BreakoutLevel::add_to_score(int points) {my_game->player_score += points;}



//=====   BreakoutGame   ====================================================================//

BreakoutGame::BreakoutGame():
level_finished(false),
player_score(0), balls_left(3), curr_level(1)
{
	level = new BreakoutLevel(this, curr_level);
}

void BreakoutGame::initialize()     {system->add_particle(level);}
void BreakoutGame::update(float dt)
{
	time_played += dt;
	if (balls_left < 0) {
		// Show gameover message
		// Play gameover sound
		// Save highscore
		// Get back to main menu
	}
	if (level_finished) {
		// Show level complete message
		// Play level finished sound

		level->life = 0;
		if (curr_level == 3) {
			// Game finished message
			// Play game finished sound
			// Save highscore
			// Get back to main menu
		}
		else {
			curr_level++;
			// Advance to the next level
			level = new BreakoutLevel(this, curr_level);
			system->add_particle(level);
			level_finished = false;
		}
	}
}

void BreakoutGame::draw(BITMAP *bitmap)
{
	draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BORDER_BMP]).dat, 0, 0);
	textprintf(bitmap, font, 528, 40,  makecol(100,100,100), "points");
	textprintf(bitmap, font, 528, 70,  makecol(100,100,100), "level");
	textprintf(bitmap, font, 528, 100, makecol(100,100,100), "balls left");
	textprintf(bitmap, font, 528, 53,  makecol(200,100,100), " %d", player_score);
	textprintf(bitmap, font, 528, 83,  makecol(100,200,100), " %d", curr_level);
	textprintf(bitmap, font, 528, 113, makecol(100,100,200), " %d", balls_left);
}



//=====   Brick   ===========================================================================//

Brick::Brick(BreakoutLevel *imy_level, float ix, float iy, int ibrick_type):
my_level(imy_level),
brick_type(ibrick_type)
{
	type = P_BRICK;
	x = ix; y = iy;
	w = ((RLE_SPRITE*)(datafile[BRICK01_BMP]).dat)->w;
	h = ((RLE_SPRITE*)(datafile[BRICK01_BMP]).dat)->h;
	o_type = O_RECT;
	life = 3;
	my_level->nr_of_bricks++;
}

void Brick::draw(BITMAP *bitmap)
{
	switch (brick_type) {
	case 0:
		rect(bitmap, int(x - w/2), int(y - h/2), int(x + w/2), int(y + h/2), makecol(75,0,0));
		line(bitmap, int(x - w/2), int(y - h/2), int(x + w/2), int(y + h/2), makecol(75,0,0));
		line(bitmap, int(x + w/2), int(y - h/2), int(x - w/2), int(y + h/2), makecol(75,0,0));
		break;
	case 1:
		if (life == 3) {
			draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK01_BMP]).dat, int(x - w/2), int(y - h/2));
		}
		else {
			set_trans_blender(0, 0, 0, MAX(0, int(255 * life * 4)));
			drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
			draw_trans_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK01_BMP]).dat, int(x - w/2), int(y - h/2));
			drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
		}
		break;
	case 2:  draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK02_BMP]).dat, int(x - w/2), int(y - h/2)); break;
	case 3:
		if (life == 3) draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK03_BMP]).dat,  int(x - w/2), int(y - h/2));
		else           draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK03B_BMP]).dat, int(x - w/2), int(y - h/2));
		break;
	case 4:  draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK04_BMP]).dat, int(x - w/2), int(y - h/2)); break;
	case 5:  draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK05_BMP]).dat, int(x - w/2), int(y - h/2)); break;
	case 6:  draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK06_BMP]).dat, int(x - w/2), int(y - h/2)); break;
	case 7:  draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK07_BMP]).dat, int(x - w/2), int(y - h/2)); break;
	case 8:  draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK08_BMP]).dat, int(x - w/2), int(y - h/2)); break;
	case 9:  draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK09_BMP]).dat, int(x - w/2), int(y - h/2)); break;
	case 10: draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BRICK10_BMP]).dat, int(x - w/2), int(y - h/2)); break;
	}
}

void Brick::update(float dt)
{
	switch (brick_type) {
	case 1: if (life != 3) life -= dt; break;
	}
}

void Brick::collision(Particle *cp)
{
	if (cp->type == P_BALL) {
		switch (brick_type) {
		case 1:
			if (life == 3) {
				life = 0.25;
				play_sample((SAMPLE*)(datafile[POP5_WAV]).dat, 255, 128, 1000, 0);
			}
			break;
		case 2: life -= 2; break;
		case 3: life -= 2; break;
		case 5:
			if (life == 3) {
				life = 0;
				// Spawn the coins
				play_sample((SAMPLE*)(datafile[BLIP1_WAV]).dat, 255, 128, 1000, 0);
			}
			break;
		default: life = 0;
		}
	}
}

void Brick::remove()
{
	my_level->nr_of_bricks--;
	switch (brick_type) {
	case 1: my_level->add_to_score(10); break;
	case 2: my_level->add_to_score(20); break;
	case 3: my_level->add_to_score(30); break;
	case 5: my_level->add_to_score(100); break;
	}
}



//=====   Ball   ============================================================================//

Ball::Ball(BreakoutLevel *imy_level, float ix, float iy, float idx, float idy):
my_level(imy_level)
{
	type = P_BALL;
	x = ix; dx = idx;
	y = iy; dy = idy;
	w = h = ((RLE_SPRITE*)(datafile[BALL01_BMP]).dat)->w;
	o_type = O_RECT;
	affected_by_obstacle = true;
	my_level->nr_of_balls++;
}

void Ball::update(float dt)
{
	if (x - w/2 > SCREEN_W || x + w/2 < 0 ||
		y - h/2 > SCREEN_H || y + h/2 < 0) {
		life = 0;
	}
}

void Ball::draw(BITMAP *bitmap)
{
	draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[BALL01_BMP]).dat, int(x - w/2), int(y - h/2));
}

void Ball::collision(Particle *cp)
{
	if (cp->type == P_PAD && dy > 0) {
		float velocity = sqrt(dx*dx + dy*dy);
		float angle = atan(dx / dy);
		angle += (x - cp->x) / 64;

		// Max angle with vertical axis is about 70 degrees
		angle = MAX(MIN(angle,  1.2), -1.2);

		dx = velocity * sin(angle);
		dy = velocity * cos(angle);
	}
}

void Ball::remove() {my_level->nr_of_balls--;}


//=====   Pad   =============================================================================//

Pad::Pad(float ix, float iy)
{
	type = P_PAD;
	x = ix; y = iy;
	w = ((RLE_SPRITE*)(datafile[PAD01_BMP]).dat)->w;
	h = ((RLE_SPRITE*)(datafile[PAD01_BMP]).dat)->h / 2;
	o_type = O_RECT;
}

void Pad::update(float dt)
{
	float temp = ABS(dx);
	dx = (temp - MIN(temp, 200 * dt)) * SGN(dx);
	if (key[KEY_LEFT])  dx = MAX(dx - 800 * dt, -200);
	if (key[KEY_RIGHT]) dx = MIN(dx + 800 * dt, 200);

	temp = x;
	x = MAX(MIN(x, 493 - w/2), 39 + w/2);
	if (x != temp) dx = 0;

	// On KEY_SPACE, release an attached ball
	if (key[KEY_SPACE]) {
		Particle *attached_ball = attached_balls.get_first();
		if (attached_ball) {
			float speed = 200; // pixels per second
			float angle = randf - 0.5;
			attached_ball->dx = speed * sin(angle) + 0.75 * dx;
			attached_ball->dy = -speed * cos(angle);
			attached_balls.remove(attached_ball);
		}
	}

	// Make sure attached balls are in the right position
	Particle *attached_ball = attached_balls.get_first();
	while (attached_ball) {
		attached_ball->x = x;
		attached_ball = attached_balls.get_next();
	}
}

void Pad::draw(BITMAP *bitmap)
{
	draw_rle_sprite(bitmap, (RLE_SPRITE*)(datafile[PAD01_BMP]).dat, int(x - w/2), int(y - h/2));
}

void Pad::attach_ball(Ball *the_ball)
{
	the_ball->dy = 0;
	the_ball->dx = 0;
	the_ball->y = y - h/2 - 4;
	the_ball->x = x;
	attached_balls.add_first(the_ball);
}



//=====   Block   ===========================================================================//

Block::Block(float ix_min, float iy_min, float ix_max, float iy_max)
{
	w = ix_max - ix_min;
	h = iy_max - iy_min;
	x = (ix_min + ix_max) / 2;
	y = (iy_min + iy_max) / 2;
	o_type = O_RECT;
}

void Block::draw(BITMAP *bitmap)
{
	//rect(bitmap, int(x - w/2), int(y - h/2), int(x + w/2), int(y + h/2), makecol(255,0,0));
}



//=====   Stars   ===========================================================================//

Star::Star(float ix, float iy, float idx, float idy)
{
	x = ix; dx = idx;
	y = iy; dy = idy;

	int brightness;
	float initial_speed = dy;

	dy = (randf * 0.9 + 0.1) * dy;
	if (initial_speed != 0) brightness = int(255 * (dy / initial_speed));
	else {
		life = 0;
		brightness = 0;
	}
	color = makecol(brightness, brightness, brightness);
}

void Star::update(float dt) {if (y > SCREEN_H) life = 0;}
void Star::draw(BITMAP *bitmap) {putpixel(bitmap, int(x), int(y), color);}



StarField::StarField():
time_passed(0),
time_per_star(1.0 / 20.0)
{}

void StarField::initialize()
{
	for (int i = 0; i < (SCREEN_H / 25) * int(1.0 / time_per_star); i++) {
		system->add_particle(new Star(SCREEN_W * randf, SCREEN_H * randf, 0, 50));
	}
}

void StarField::update(float dt)
{
	time_passed += dt;
	while (time_passed > time_per_star) {
		system->add_particle(new Star(SCREEN_W * randf, 0, 0, 50));
		time_passed -= time_per_star;
	}
}

