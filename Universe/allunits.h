#ifndef ALLUNITS_H
#define	ALLUNITS_H

#include <vector>
#include "tile.h"
#include "unit.h"

using std::vector;

class unit;
class hiveMind;

class allunits
{
    friend int main();
    friend class tile;
    friend void unit::infect();
    friend void unit::reproduce(int);
    friend void unit::die();
    friend void unit::giveBirth();
    friend class hiveMind;
    friend class object;
private:
    vector<vector<unit> > data;
public:
    allunits();
    unit* get(unit* u, int index);
    unit* get(hiveMind* h, int index);
};

#endif	/* ALLUNITS_H */

