/**********************************************************************************************
 *
 *  Type O' Particles
 *
 */

#pragma once

#include "base.h"
#include "p_engine.h"

#include <vector>

// Defined particle types
enum class ParticleType : uint8_t {
	Ball  = 1,
	Brick = 2,
	Pad   = 3
};

// Backward compatibility constants (allow existing code using P_BALL etc.)
inline constexpr int P_BALL  = static_cast<int>(ParticleType::Ball);
inline constexpr int P_BRICK = static_cast<int>(ParticleType::Brick);
inline constexpr int P_PAD   = static_cast<int>(ParticleType::Pad);

class BreakoutGame;
class Pad;

//=====   BreakoutLevel   ===================================================================//

class BreakoutLevel : public Particle {
public:
	BreakoutLevel(BreakoutGame *my_game, int level_nr);
	void initialize() override;
	void update(float dt) override;
	void draw() override;
	void remove() override;

	void add_to_score(int points);

	int nr_of_bricks = 0;
	int nr_of_balls = 0;
private:
	Particle_System level;
	BreakoutGame *my_game = nullptr;
	Pad *pad = nullptr;
};

//=====   BreakoutGame   ====================================================================//

class BreakoutGame : public Particle {
public:
	BreakoutGame();
	void initialize() override;
	void update(float dt) override;
	void draw() override;

	bool level_finished = false;
	int player_score = 0;
	int balls_left = 3;
private:
	float time_played = 0.f;
	int curr_level = 1;
	BreakoutLevel *level = nullptr;
};

//=====   Brick   ===========================================================================//

class Brick : public Particle {
public:
	Brick(BreakoutLevel *my_level, float x, float y, int brick_type);
	void draw() override;
	void update(float dt) override;
	void collision(Particle *cp) override;
	void remove() override;
private:
	BreakoutLevel *my_level = nullptr;
	unsigned short brick_type = 0;
};

//=====   Ball   ============================================================================//

class Ball : public Particle {
public:
	Ball(BreakoutLevel *my_level, float ix, float iy, float idx, float idy);
	void update(float dt) override;
	void draw() override;
	void collision(Particle *cp) override;
	void remove() override;
private:
	BreakoutLevel *my_level = nullptr;
};

//=====   Pad   =============================================================================//

class Pad : public Particle {
public:
	Pad(float ix, float iy);
	void update(float dt) override;
	void draw() override;

	void attach_ball(Ball *the_ball);
private:
	std::vector<Particle*> attached_balls;
};

//=====   Block   ===========================================================================//

class Block : public Particle {
public:
	Block(float ix_min, float iy_min, float ix_max, float iy_max);
	void draw() override;
};

//=====   Stars   ===========================================================================//

class Star : public Particle {
public:
	Star(float x, float y, float dx, float dy);
	void update(float dt) override;
	void draw() override;
private:
	Color color = rgb(255, 255, 255);
};

class StarField : public Particle {
public:
	StarField() = default;
	void initialize() override;
	void update(float dt) override;
private:
	float time_passed = 0.f;
	float time_per_star = 1.f / 40.f;
};
