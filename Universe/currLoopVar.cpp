#include "currLoopVar.h"

bool curLoopVar::created=false;

curLoopVar::curLoopVar()
{
    if(curLoopVar::created==true)
        return;
    hivePlayer=0;
    hiveIndex=0;
    unitPlayer=0;
    unitIndex=0;
    curLoopVar::created=true;
}
