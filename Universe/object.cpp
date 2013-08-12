#include "object.h"
#include "unit.h"
#include "currLoopVar.h"
#include "globals.h"
#include "hivemind.h"

#include <cmath>

objectDescriptor::objectDescriptor(short w, short wv, short pid, bool walk, bool sleep)
{
    weight=w;
    weightVariation=wv;
    id=pid;
    walkable=walk;
    sleepable=sleep;
}

object::object(short w, short p, short i, short px, short py, short what)
{
    weight=w;
    heldByPlayer=p;
    heldByIndex=i;
    y=py;
    x=px;
    whatIsIt=what;
}

#define Z(type, val) \
    type object::get ## val(unit *u) \
    { \
        if(u->player==curLoops.unitPlayer && u->index==curLoops.unitPlayer) \
        { \
            if((heldByPlayer==u->player && heldByIndex==u->index) || (abs(x-u->x)<=1 && abs(y-u->y)<=1)) \
                return val; \
        } \
        return -9999; \
    } 
    LISTVARSOBJVAR
#undef Z

#define Z(type, val) \
    type object::get ## val(unit *u) \
    { \
        if(u->player==curLoops.unitPlayer && u->index==curLoops.unitPlayer) \
        { \
            if((heldByPlayer==u->player && heldByIndex==u->index) || (abs(x-u->x)<=1 && abs(y-u->y)<=1)) \
                return allObjectDesc[whatIsIt].val; \
        } \
        return -9999; \
    } 
    LISTVARSOBJSTAT
#undef Z
            
#define Z(type, val) \
    type object::get ## val(hiveMind *h, short wx, short wy) \
    { \
        if(h->player==curLoops.hivePlayer && h->index==curLoops.hivePlayer) \
        { \
            if(abs(wy-h->centery)<h->range && abs(wx-h->centerx)<h->range) \
            { \
                return val; \
            } \
        } \
        return -9999; \
    } 
    LISTVARSOBJVAR
#undef Z

#define Z(type, val) \
    type object::get ## val(hiveMind *h, short wx, short wy) \
    { \
        if(h->player==curLoops.hivePlayer && h->index==curLoops.hivePlayer) \
        { \
            if(abs(wy-h->centery)<h->range && abs(wx-h->centerx)<h->range) \
            { \
                return allObjectDesc[whatIsIt].val; \
            } \
        } \
        return -9999; \
    } 
    LISTVARSOBJSTAT
#undef Z
            
#define Z(type, val) \
    type object::get ## val(hiveMind *h, short unitIndex) \
    { \
        if(h->player==curLoops.hivePlayer && h->index==curLoops.hivePlayer) \
        { \
            if(abs(allUnits.data[h->player][unitIndex].y - h->centery)<h->range && abs(allUnits.data[h->player][unitIndex].x - h->centerx)<h->range) \
            { \
                return val; \
            } \
        } \
        return -9999; \
    } 
    LISTVARSOBJVAR
#undef Z

#define Z(type, val) \
    type object::get ## val(hiveMind *h, short unitIndex) \
    { \
        if(h->player==curLoops.hivePlayer && h->index==curLoops.hivePlayer) \
        { \
            if(abs(allUnits.data[h->player][unitIndex].y - h->centery)<h->range && abs(allUnits.data[h->player][unitIndex].x - h->centerx)<h->range) \
            { \
                return allObjectDesc[whatIsIt].val; \
            } \
        } \
        return -9999; \
    } 
    LISTVARSOBJSTAT
#undef Z

/*short object::getWeight(unit *u)
{
    if(u->player==curLoops.unitPlayer && u->index==curLoops.unitPlayer)
    {
        if((heldByPlayer==u->player && heldByIndex==u->index) || (abs(x-u->x)<=1 && abs(y-u->y)<=1))
            return weight;
    }
    return -9999;
}*/