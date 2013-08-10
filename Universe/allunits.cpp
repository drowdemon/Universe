#include "allunits.h"
#include "globals.h"
#include "unit.h"
#include "hivemind.h"
#include <cmath>

allunits::allunits()
{
    data.resize(NUMPLAYERS);
}

unit* allunits::get(unit* u, int index) //should only be used with current unit
{
    int p=u->player;
    if(p!=curLoops.unitPlayer || u->index!=curLoops.unitIndex)
        return NULL;
    if(mapseenunit[p][data[p][index].y][data[p][index].x].get()) //can see them
        return &data[p][index];
    return NULL;
}
unit* allunits::get(hiveMind* h, int index)
{
    int p=h->player;
    if(p!=curLoops.hivePlayer || h->index!=curLoops.hiveIndex)
        return NULL;
    if(mapseenhive[p][h->index][data[p][index].y][data[p][index].x].get()) //can see them
        return &data[p][index];
    return NULL;
}