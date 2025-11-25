/**********************************************************************************************
 *
 *  Type O' Particles
 *
 */


#ifndef INCLUDED_PTYPES_H
#define INCLUDED_PTYPES_H

#include "base.h"
#include "p_engine.h"

#include <vector>

// Defined particle types
#define P_BALL		1
#define P_BRICK		2
#define P_PAD		3


// Empty class definition
class BreakoutGame;
class Pad;



//=====   BreakoutLevel   ===================================================================//

class BreakoutLevel : public Particle {
public:
	BreakoutLevel(BreakoutGame *my_game, int level_nr);
	void initialize();
	void update(float dt);
	void draw();
	void remove();

	void add_to_score(int points);

	int nr_of_bricks;
	int nr_of_balls;
private:
	Particle_System level;
	BreakoutGame *my_game;
	Pad *pad;
};



//=====   BreakoutGame   ====================================================================//

class BreakoutGame : public Particle {
public:
	BreakoutGame();
	void initialize();
	void update(float dt);
	void draw();

	bool level_finished;
	int player_score;
	int balls_left;
private:
	BreakoutLevel *level;
	float time_played;
	int curr_level;
};



//=====   Brick   ===========================================================================//

class Brick : public Particle {
public:
	Brick(BreakoutLevel *my_level, float x, float y, int brick_type);
	void draw();
	void update(float dt);
	void collision(Particle *cp);
	void remove();
private:
	BreakoutLevel *my_level;
	unsigned short brick_type;
};



//=====   Ball   ============================================================================//

class Ball : public Particle {
public:
	Ball(BreakoutLevel *my_level, float ix, float iy, float idx, float idy);
	void update(float dt);
	void draw();
	void collision(Particle *cp);
	void remove();
private:
	BreakoutLevel *my_level;
};



//=====   Pad   =============================================================================//

class Pad : public Particle {
public:
	Pad(float ix, float iy);
	void update(float dt);
	void draw();

	void attach_ball(Ball *the_ball);
private:
	std::vector<Particle*> attached_balls;
};



//=====   Block   ===========================================================================//

class Block : public Particle {
public:
	Block(float ix_min, float iy_min, float ix_max, float iy_max);
	void draw();
};



//=====   Stars   ===========================================================================//

class Star : public Particle {
public:
	Star(float x, float y, float dx, float dy);
	void update(float dt);
	void draw();
private:
	Color color;
};


class StarField : public Particle {
public:
	StarField();
	void initialize();
	void update(float dt);
private:
	float time_passed, time_per_star;
};



#endif /* INCLUDED_PTYPES_H */
