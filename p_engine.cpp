/**********************************************************************************************
 *  Particle engine by Bjørn Lindeijer
 *  Version 0.9
 *
 *  Changes:
 *   0.9: Changed the way particles are defined. Particles should now be derived from
 *        the base Particle class.
 *   0.8: More gravity support and bounding boxes.
 *   0.7: Added gravity support and time.
 *   0.6: Particles are initialized/updated/drawn with their own user-defined function.
 *   0.5: Initial particle system, with some built-in particles.
 */

#include "p_engine.h"
#include "base.h"

//=====   Particle class   ==================================================================//

Particle::Particle() = default;


//=====   Particle System class   ===========================================================//

Particle_System::Particle_System() = default;

Particle_System::~Particle_System()
{
	remove_particles();
}

void Particle_System::add_particle(Particle *p)
{
	if (first_particle) first_particle->prev = p;
	p->next = first_particle;
	p->system = this;
	first_particle = p;
	nr_of_particles++;

	p->initialize();
	set_obstacle(p);
	set_grav_source(p);
}

void Particle_System::remove_particle(Particle *p)
{
	if (p->prev) p->prev->next = p->next;
	else first_particle = p->next;
	if (p->next) p->next->prev = p->prev;

	p->g_type = GravityType::None;
	p->o_type = ObstacleType::None;
	set_obstacle(p);
	set_grav_source(p);
	p->remove();
	delete p;

	nr_of_particles--;
}

void Particle_System::remove_particles()
{
	while (first_particle) remove_particle(first_particle);
}

void Particle_System::update_particles(float dt)
{
	Particle* p = first_particle;
	Particle* temp_p;
	float Dx, Dy, squares;
	bool bounce_x, bounce_y;

	while (p) {
		if (p->life <= 0) {
			temp_p = p;
			p = p->next;
			remove_particle(temp_p);
		}
		else {
			if (p->gravity > 0) {
				// Calculate effect of gravity sources on this particle.
				Modifier *g = first_grav_source;
				while (g) {
					if (g->p != p) {
						switch (g->p->g_type) {
						case GravityType::Constant:
							p->dx += p->gravity * g->p->gx * dt;
							p->dy += p->gravity * g->p->gy * dt;
							break;
						case GravityType::Point:
							Dx = g->p->x - p->x;
							Dy = g->p->y - p->y;
							squares = (Dx * Dx + Dy * Dy);
							p->dx += p->gravity * (g->p->gx / squares) / sqrt(squares) * Dx * dt;
							p->dy += p->gravity * (g->p->gy / squares) / sqrt(squares) * Dy * dt;
							break;
						case GravityType::Line:
							Dx = g->p->x - p->x;
							Dy = g->p->y - p->y;
							p->dx += p->gravity * (g->p->gx / (Dx * Dx * SGN(Dx))) * dt;
							p->dy += p->gravity * (g->p->gy / (Dy * Dy * SGN(Dy))) * dt;
							break;
						case GravityType::None:
							break;
						}
					}
					g = g->next;
				}
			}
			if (p->affected_by_obstacle) {
				// Check for collisions with other particles and calculate the result.
				Modifier *o = first_obstacle;
				bounce_x = false;
				bounce_y = false;
				//p->colliding_particles.clear();
				bool colliding = false;
				while (o) {
					if (o->p != p) {
						switch (o->p->o_type) {
						case ObstacleType::Rect:
							if ((p->x + p->w / 2) > (o->p->x - o->p->w / 2) &&
								(p->y + p->h / 2) > (o->p->y - o->p->h / 2) &&
								(p->x - p->w / 2) < (o->p->x + o->p->w / 2) &&
								(p->y - p->h / 2) < (o->p->y + o->p->h / 2)) {

								//p->colliding_particles.push_back(o->p);
								colliding = true;

								if (!p->colliding /*_particles->is_in(o->p)*/) {
									if      (p->dy == 0) bounce_x = true;
									else if (p->dx == 0) bounce_y = true;
									else {
										if (p->dx < 0) Dx = (p->x - p->w / 2) - (o->p->x + o->p->w / 2);
										else           Dx = (p->x + p->w / 2) - (o->p->x - o->p->w / 2);

										if (p->dy < 0) Dy = (p->y - p->h / 2) - (o->p->y + o->p->h / 2);
										else           Dy = (p->y + p->h / 2) - (o->p->y - o->p->h / 2);

										if (SGN(p->dx) == SGN(p->dy)) {
											if (Dx / Dy < p->dx / p->dy) bounce_x = true;
											else bounce_y = true;
										}
										else {
											if (Dx / Dy < p->dx / p->dy) bounce_y = true;
											else bounce_x = true;
										}
									}
									p->collision(o->p);
									o->p->collision(p);
								}
							}
							break;
						case ObstacleType::None:
							break;
						}
					}
					o = o->next;
				}
				if (bounce_x) p->dx *= -1;
				if (bounce_y) p->dy *= -1;
				p->colliding = colliding;
			}

			p->x += p->dx * dt;
			p->y += p->dy * dt;
			p->update(dt);
			set_obstacle(p);
			set_grav_source(p);

			p = p->next;
		}
	}
}

void Particle_System::draw_particles()
{
	for (Particle *p = first_particle; p; p = p->next)
		p->draw();
}

void Particle_System::set_obstacle(Particle *p)
{
	if (p->obstacle && p->o_type == ObstacleType::None) {
		// Remove obstacle from list
		if (p->obstacle->prev) p->obstacle->prev->next = p->obstacle->next;
		else first_obstacle = p->obstacle->next;
		if (p->obstacle->next) p->obstacle->next->prev = p->obstacle->prev;
		delete p->obstacle;
		p->obstacle = nullptr;
	}
	else if (!p->obstacle && p->o_type != ObstacleType::None) {
		// Add obstacle to list
		auto *new_o = new Modifier(p);
		if (first_obstacle) {
			first_obstacle->prev = new_o;
			new_o->next = first_obstacle;
			first_obstacle = new_o;
		}
		else {
			first_obstacle = new_o;
		}
		p->obstacle = new_o;
	}
}

void Particle_System::set_grav_source(Particle *p)
{
	if (p->grav_source && p->g_type == GravityType::None) {
		// Remove gravity source from list
		if (p->grav_source->prev) p->grav_source->prev->next = p->grav_source->next;
		else first_grav_source = p->grav_source->next;
		if (p->grav_source->next) p->grav_source->next->prev = p->grav_source->prev;
		delete p->grav_source;
		p->grav_source = nullptr;
	}
	else if (!p->grav_source && p->g_type != GravityType::None) {
		// Add gravity source to list
		auto *new_g = new Modifier(p);
		if (first_grav_source) {
			first_grav_source->prev = new_g;
			new_g->next = first_grav_source;
			first_grav_source = new_g;
		}
		else {
			first_grav_source = new_g;
		}
		p->grav_source = new_g;
	}
}
