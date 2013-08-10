#include "hivemind.h"
#include "globals.h"
#include <cmath>

hiveMind::hiveMind(int cx, int cy, int r, int p, int i)
{
    centerx=cx;
    centery=cy;
    range=r;
    player=p;
    index=i;
}

void hiveMind::act()
{
}

//getters
#define X(type, val) \
    type hiveMind::get ## val() \
    { \
        if(curLoops.hivePlayer==player && curLoops.hiveIndex==index) \
                return val ;  \
        return -1; \
    } //-1 is err
    LISTVARSHIVE
#undef X

//getters for units in hive

#define Y(type, val) \
    type hiveMind::getUnitInHive ## val(int index) \
    { \
        if(curLoops.hiveIndex==index && curLoops.hivePlayer==player) \
        { \
            if(abs(centerx-allUnits.data[player][index].x)<range && abs(centery-allUnits.data[player][index].y)<range) \
                return allUnits.data[player][index].val; \
        } \
        return -127; \
    } 
    LISTVARSUNIT
#undef Y
