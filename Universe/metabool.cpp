#include "metabool.h"
#include "currLoopVar.h"
#include "globals.h"

metabool::metabool()
{
    b=0;
}

unsigned char metabool::get(unit *u)
{
    if(u->player==curLoops.unitPlayer && u->index==curLoops.unitIndex)
        return b;
    return -127; //err
}

unsigned char metabool::get(hiveMind *h)
{
    if(h->player==curLoops.hivePlayer && h->index==curLoops.hiveIndex)
        return b;
    return -127; //err
}
