#include "object.h"
#include "unit.h"
#include "currLoopVar.h"
#include "globals.h"
#include "hivemind.h"
#include "food.h"
#include "disease.h"

#include <cmath>
#include <cstdlib>

objectDescriptor::objectDescriptor(short w, short wv, short pid, bool walk, bool sleep, bool vedib, short aedib, short dis, food pf)
{
    weight=w;
    weightVariation=wv;
    id=pid;
    walkable=walk;
    sleepable=sleep;
    vaguelyEdible=vedib;
    actuallyEdible=aedib;
    disease=dis;
    possFood=pf;
}

object::object(short w, short p, short i, short px, short py, short what, bool aedib, food pf)
{
    weight=w;
    heldByPlayer=p;
    heldByIndex=i;
    y=py;
    x=px;
    whatIsIt=what;
    actuallyEdible=aedib;
    possFood=pf;
}

object::object(objectDescriptor& od, short p, short i, short px, short py)
{
    weight=od.weight+((rand()%(od.weightVariation*2+1))-od.weightVariation);
    heldByPlayer=p;
    heldByIndex=i;
    x=px;
    y=py;
    whatIsIt=od.id;
    actuallyEdible=od.actuallyEdible;
    if(od.disease!=-1)
    {
        infected.push_back(od.disease);
        infectionTime.push_back(-1); //forever and ever and ever.
    }
    possFood=od.possFood;
}

bool object::rot()
{
    for(unsigned int i=0; i<infectionTime.size(); i++)
    {
        if(infectionTime[i]>allDiseases[infected[i]].duration && allDiseases[infected[i]].duration!=-1)
        {
            infectionTime.erase(infectionTime.begin()+i);
            infected.erase(infected.begin()+i);
            i--;
        }
    }
    if(!possFood)
        return true;
    vector<int> dis;
    dis=possFood.rot();
    if(dis.size()==1 && dis[0]==-1) //destroy this, it biodegraded
        return false; //existence failure
    for(unsigned int i=0; i<dis.size(); i++)
    {
        bool good=true;
        for(unsigned int j=0; j<infected.size(); j++)
        {
            if(infected[j]==dis[i])
            {
                infectionTime[j]=0;
                good=false;
                break;
            }
        }
        if(good)
        {
            infected.push_back(dis[i]);
            infectionTime.push_back(0);
        }
    }
    return true;
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