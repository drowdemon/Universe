#include "allminds.h"
#include "globals.h"
#include "unit.h"
#include "currLoopVar.h"
#include <cmath>

allminds::allminds()
{
    data.resize(NUMPLAYERS);
}

hiveMind* allminds::get(unit* u, int index) //can only be used with current unit
{
    int p=u->player;
    if(p!=curLoops.unitPlayer || u->index!=curLoops.unitIndex)
        return NULL;
    if(abs(u->x-data[p][index].centerx)<data[p][index].range && abs(u->y-data[p][index].centery)<data[p][index].range)
        return &data[p][index];
    return NULL;
}
hiveMind* allminds::get(hiveMind* h, int index)
{
    int p=h->player;
    if(p!=curLoops.hivePlayer || h->index!=curLoops.hiveIndex)
        return NULL;
    if(abs(h->centerx-data[p][index].centerx)<data[p][index].range && abs(h->centery-data[p][index].centery)<data[p][index].range)
        return &data[p][index];
    return NULL;
}
