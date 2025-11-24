/**********************************************************************************************
 *
 *  Particle engine header
 *
 */


#ifndef INCLUDED_P_ENGINE_H
#define INCLUDED_P_ENGINE_H


#include "base.h"


// For a random number from 0 to 0.999 (equal chances for every number)
#define randf	(float(rand()%1024) / 1024.0)


// Gravitation types
#define G_NONE		0
#define G_CONSTANT	1
#define G_POINT		2
#define G_LINE		3

// Obstacle types
#define O_NONE		0
#define O_RECT		1



// Empty class declarations
class Particle_System;
class Modifier;
class ParticleList;


//=====   Particle class   ==================================================================//

/*
  Derive your own particles from this base Particle class. Like:
  class MyParticle : public Particle {};

  When you want to add your particle to the system, create an instance of your particle and
  give it to the system with Particle_System::add_particle(Particle *p);. From that time on
  you should not be using the pointer to your particle, nor try to delete it. The system will
  take care of the particle now.
  You might want to call Particle_System::remove_particle(Particle *p) to remove the particle
  from the system, but you'll have to be really sure that the particle still exists.

  The system will call the following functions:

   initialize();            -> when the particle has been added to the system.
   update(float dt);        -> when update_particles(float dt) has reached this particle.
   draw();                  -> when draw_particles() has reached this particle.
   collision(Particle *p);  -> when a collision occurs between this particle and *p.
   remove();                -> just before the particle is deleted.

  Also, the system checks the g_type and o_type parameters after each update and will adapt
  automatically to get the expected result.
*/

class Particle {
public:
	Particle();
	virtual ~Particle() {};

	virtual void initialize() {};
	virtual void update(float dt) {};
	virtual void draw() {};
	virtual void collision(Particle *p) {};
	virtual void remove() {};

	float x, y, dx, dy, life;		// If life <= 0 then the particle will be removed.
	float gravity;					// Amount of invluence from gravity sources.
	bool affected_by_obstacle;

	int type;						// Can be used to identity the particle, 0 by default.
	float w, h, r, gx, gy;			// Width, Height, Radius and G-Force.
	int g_type, o_type;				// Gravity type and Obstacle type.

	Particle_System *system;		// Can be used to add additional particles to the system.

	/*
	  The following variables are used by the particle system
	  and should not be modified by the particle.
	  It would be better if only the system had acces
	  to these variables.
	*/
	Particle *prev, *next;
	Modifier *obstacle;
	Modifier *grav_source;
	//ParticleList *colliding_particles;
	bool colliding;
};



//=====   Particle System class   ===========================================================//


class Modifier {
public:
	Modifier(Particle *ip) : p(ip), prev(NULL), next(NULL) {}
	Particle *p;
	Modifier *prev, *next;
};

class Particle_System {
public:
	Particle_System();
	~Particle_System();

	void add_particle(Particle *p);
	void remove_particle(Particle *p);

	void draw_particles();
	void update_particles(float dt);
	void remove_particles();

	void set_obstacle(Particle *p);
	void set_grav_source(Particle *p);

	unsigned int nr_of_particles;
private:
	Particle *first_particle;
	Modifier *first_obstacle;
	Modifier *first_grav_source;
};



#endif /* INCLUDED_P_ENGINE_H */
