#include "commWrapper.h"
#include "currLoopVar.h"
#include "unit.h"
#include "globals.h"
#include <vector>

using namespace std;

commWrapper::commWrapper()
{
}

void commWrapper::push(communicationProtocol& cp)
{
    communiques.push_back(cp);
}

communicationProtocol commWrapper::read(unit *u, int index)
{
    if(index==u->index && index==curLoops.unitIndex && player==u->player && u->player==curLoops.unitPlayer)
        return communiques[index];
    return communicationProtocol();
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