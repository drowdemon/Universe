#ifndef ALLUNITS_H
#define	ALLUNITS_H

#include <vector>
#include "tile.h"
#include "unit.h"

using std::vector;

class unit;
class hiveMind;
class commWrapper;

class allunits
{
    friend int main();
    friend class tile;
    friend class creature;
    friend void unit::reproduce(int);
    friend void unit::die();
    friend void unit::giveBirth();
    friend void unit::learn();
    friend creature* unit::createFetus(int);
    friend class hiveMind;
    friend class object;
    friend void reformat();
	friend class commWrapper;
	friend void creature::reproduce(int, creature*);
private:
    vector<vector<unit*> > data; //It stores pointers to units to save stack space. 
public:
    allunits();
    unit* get(unit* u, int index, int player);
    unit* get(hiveMind* h, int index);
    ~allunits(); //You could just destroy one of the game's main data storage variables, but then it would crash, and the developer doing the destruction wouldn't gain a thing.
};

#endif	/* ALLUNITS_H */

