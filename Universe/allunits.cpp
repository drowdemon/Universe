#include "allunits.h"
#include "globals.h"
#include "unit.h"
#include "hivemind.h"
#include <cmath>

allunits::allunits()
{
    data.resize(NUMPLAYERS);
}

unit* allunits::get(unit* u, int index, int player) //can get any other unit, but only pass it this unit. MUST DELETE RESULTS
{
    int p=u->player;
    if(p!=curLoops.unitPlayer || u->index!=curLoops.unitIndex)
        return NULL;
    if(mapseenunit[p][data[player][index]->y][data[player][index]->x].get(u)==1) //can see them
    {
        unit* ret=new unit;
        *ret=*data[player][index];
        return ret;
    }
    return NULL;
}
unit* allunits::get(hiveMind* h, int index) //MUST DELETE RESULTS
{
    int p=h->player;
    if(p!=curLoops.hivePlayer || h->index!=curLoops.hiveIndex)
        return NULL;
    if(mapseenhive[p][h->index][data[p][index]->y][data[p][index]->x].get(h)==1) //can see them
    {
        unit* ret=new unit;
        *ret=*data[p][index];
        return ret;
    }
    return NULL;
}

allunits::~allunits()
{
    for(unsigned int i=0; i<data.size(); i++)
    {
        for(unsigned int j=0; j<data[i].size(); j++)
        {
            delete data[i][j];
            data[i][j]=NULL;
        }
    }
}