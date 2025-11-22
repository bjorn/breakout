/**********************************************************************************************
 *
 *  ParticleList class
 *  By Bjørn Lindeijer
 *
 */

#ifndef INCLUDED_PARTICLE_LIST_H
#define INCLUDED_PARTICLE_LIST_H

#include "p_engine.h"


//=====   List class   ======================================================================//

class ParticleList {
public:
	ParticleList():first(NULL), last(NULL), curr_item(NULL), item_count(0) {}
	~ParticleList() {remove_all();}

	void add_first(Particle* value);
	void add_last(Particle* value);
	void remove(Particle* value);
	void remove_all();
	bool is_in(Particle* value) const;
	bool is_empty() const {return first == NULL;}
	int  nr_of_items() const {return item_count;}

	Particle* get_first();
	Particle* get_next();
	Particle* get_element(int nr);

private:
	class ListItem {
	public:
		ListItem(Particle* value, ListItem *pred = NULL, ListItem *succ = NULL):val(value), prev(pred), next(succ) {}
		~ListItem() {if (prev) prev->next = next; if (next) next->prev = prev;}
		Particle* val;
		ListItem *prev, *next;
	};

	ListItem *first;
	ListItem *last;
	ListItem *curr_item;
	int item_count;
};


#endif /* INCLUDED_PARTICLE_LIST_H */
