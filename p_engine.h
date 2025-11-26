/**********************************************************************************************
 *
 *  Particle engine header
 *
 */

#pragma once

#include <cstdint>
#include <cstdlib>

// Random float in [0,1)
inline float randf()
{
    return static_cast<float>(std::rand() & 1023) / 1024.0f;
}

// Gravitation types
enum class GravityType : uint8_t {
    None,
    Constant,
    Point,
    Line
};

// Obstacle types
enum class ObstacleType : uint8_t {
    None,
    Rect
};

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
    virtual ~Particle() = default;

    virtual void initialize() {};
    virtual void update(float dt) {};
    virtual void draw() {};
    virtual void collision(Particle *p) {};
    virtual void remove() {};

    float x = 0.f, y = 0.f, dx = 0.f, dy = 0.f;
    float life = 1.f; // If life <= 0 then the particle will be removed.
    float gravity = 0.f; // Amount of influence from gravity sources.
    bool affected_by_obstacle = false;

    int type = 0; // Can be used to identify the particle, 0 by default.
    float w = 0.f, h = 0.f; // Width, Height
    float r = 0.f; // Radius
    float gx = 0.f, gy = 0.f; // G-Force
    GravityType g_type = GravityType::None; // Gravity type
    ObstacleType o_type = ObstacleType::None; // Obstacle type

    Particle_System *system = nullptr; // Can be used to add additional particles to the system.

    /*
      The following variables are used by the particle system
      and should not be modified by the particle.
      It would be better if only the system had access to these variables.
    */
    Particle *prev = nullptr;
    Particle *next = nullptr;
    Modifier *obstacle = nullptr;
    Modifier *grav_source = nullptr;
    // std::vector<Particle*> colliding_particles;
    bool colliding = false;
};

//=====   Particle System class   ===========================================================//

class Modifier {
public:
    Modifier(Particle *ip)
        : p(ip)
    {
    }

    Particle *p;
    Modifier *prev = nullptr;
    Modifier *next = nullptr;
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

    unsigned int nr_of_particles = 0;

private:
    Particle *first_particle = nullptr;
    Modifier *first_obstacle = nullptr;
    Modifier *first_grav_source = nullptr;
};
