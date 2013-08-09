#ifndef HIVEMIND_H
#define	HIVEMIND_H

#include "unit.h"

#define LISTVARS \
        X(int, centerx) \
        X(int, centery) \
        X(int, range) \
        X(int, player) \
        X(int, index)

class hiveMind
{
    friend void unit::seehive(int hiveindex);
    friend void unit::unseehive(int hiveindex);
private: 
    int centerx;
    int centery;
    int range; //dependant on tech, num people.
    int player;
    int index;
    hiveMind(int cx, int cy, int r, int p, int i);
public:    
    virtual void act();
    friend class tile; //a tile can access anything in here. No need for the universe to hide data from itself
    //below are getters
#define X(type, val) \
    type get ## val() ;
    LISTVARS
#undef X
};

#endif	/* HIVEMIND_H */

