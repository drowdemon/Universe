#ifndef HIVEMIND_H
#define	HIVEMIND_H

#include "unit.h"
#include "object.h"

#define LISTVARSHIVE \
        X(int, centerx) \
        X(int, centery) \
        X(int, range) \
        X(int, player) \
        X(int, index)

class hiveMind
{
    friend class allunits;
    friend class allminds;
    friend class unit; //friendship not inherited, and both of these are inherited from, so its ok.
    friend void unit::seehive(int hiveindex);
    friend void unit::unseehive(int hiveindex);
    friend class tile; //a tile can access anything in here. No need for the universe to hide data from itself
    friend class object;
	friend class metabool;
private: 
    int centerx;
    int centery;
    int range; //dependant on tech, num people.
    int player;
    int index;
    hiveMind(int cx, int cy, int r, int p, int i);
	hiveMind(); //DO NOT USE, LIKE unit DEFAULT CONSTRUCTOR
public:    
	hiveMind& operator=(const hiveMind &source);
    virtual void act();
    //below are getters
#define X(type, val) \
    type get ## val() ;
    LISTVARSHIVE
#undef X
    //getters for units in hive mind
	unit* getUnitInHive(int index);
    //vector<object> getUnitInHivecarrying(int index);
        
/*#define Y(type, val) \
    type getUnitInHive ## val(int index);
    LISTVARSUNIT
#undef Y*/
};

#endif	/* HIVEMIND_H */

