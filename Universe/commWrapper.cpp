#include "commWrapper.h"
#include "currLoopVar.h"
#include "unit.h"
#include "globals.h"
#include <vector>

using namespace std;

commWrapper::commWrapper()
{
}

void commWrapper::push(communicationProtocol& cp, unit *u)
{
    if(cp.whoAsks->index==u->index && cp.whoAsks->player==u->player && u->index==curLoops.unitIndex && u->player==curLoops.unitPlayer && (abs(u->x-allUnits.data[player][index]->x)+abs(u->y-allUnits.data[player][index]->y)) < SPEAKDIST)
        communiques.push_back(cp);
}

communicationProtocol commWrapper::read(unit *u, int index)
{
    if(index==u->index && index==curLoops.unitIndex && player==u->player && player==curLoops.unitPlayer)
        return communiques[index];
    return communicationProtocol(u);
}

void commWrapper::popBack(unit*u)
{
    if(index==u->index && index==curLoops.unitIndex && player==u->player && u->player==curLoops.unitPlayer)
        communiques.pop_back();
}

void commWrapper::erase(unit* u, int index)
{
    if(index==u->index && index==curLoops.unitIndex && player==u->player && u->player==curLoops.unitPlayer)
        communiques.erase(communiques.begin()+index);
}