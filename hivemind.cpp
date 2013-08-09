#include "hivemind.h"

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
    type hiveMind::get ## val() {return val ;}
    LISTVARS
#undef X