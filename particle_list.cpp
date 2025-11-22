/**********************************************************************************************
 *
 *  ParticleList class
 *  By Bjørn Lindeijer
 *
 */


#include "particle_list.h"



//=====   List class   ======================================================================//

void ParticleList::add_first(Particle* value)
{
	ListItem *new_item = new ListItem(value, first);
	if (last == NULL) last = new_item;
	first = new_item;
	item_count++;
}

void ParticleList::add_last(Particle* value)
{
	ListItem *new_item = new ListItem(value);
	if (first == NULL) first = new_item;
	else last->next = new_item;
	last = new_item;
	item_count++;
}

void ParticleList::remove(Particle* value)
{
	ListItem *pointer = first;
	while (pointer) {
		if (pointer->val == value) {
			if (pointer == first) first = pointer->next;
			if (pointer == last) last = pointer->prev;
			delete pointer;
			pointer = NULL;
			item_count--;
		}
		else pointer = pointer->next;
	}
}

void ParticleList::remove_all()
{
	ListItem *pointer = first;
	ListItem *temp = pointer;
	while (temp) {
		temp = pointer;
		pointer = pointer->next;
		delete temp;
	}
	first = last = curr_item = NULL;
	item_count = 0;
}

bool ParticleList::is_in(Particle* value) const
{
	ListItem *pointer = first;
	while (pointer) {
		if (pointer->val == value) return true; 
		pointer = pointer->next;
	}
	return false;
}


Particle* ParticleList::get_first()
{
	curr_item = first;
	return (curr_item) ? curr_item->val : NULL;
}

Particle* ParticleList::get_next()
{
	if (curr_item) {curr_item = curr_item->next;}
	return (curr_item) ? curr_item->val : NULL;
}

Particle* ParticleList::get_element(int nr)
{
	int index = 0;
	curr_item = first;
	while (curr_item && index < nr) {
		curr_item = curr_item->next;
		index++;
	}
	return (curr_item) ? curr_item->val : NULL;
}
