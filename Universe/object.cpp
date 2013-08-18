#include "object.h"
#include "unit.h"
#include "currLoopVar.h"
#include "globals.h"
#include "hivemind.h"
#include "food.h"
#include "disease.h"
#include "throw.h"

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

object::object(short w, short p, short i, short px, short py, short what, bool aedib, food pf, short ind)
{
    weight=w;
    heldByPlayer=p;
    heldByIndex=i;
    y=py;
    x=px;
    whatIsIt=what;
    actuallyEdible=aedib;
    possFood=pf;
    speed=0;
    toX=toY=-1;
    index=ind;
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

void object::move()
{
    int dist=speed/1000+(((rand()%1000)<(speed%1000)) ? 1 : 0);
    int dx=0;
    int dy=0;
    if(dist<sqrt(pow(x-toX,2)+pow(y-toY,2)))
    {
        dx=toX-x;
        dy=toY-y;
    }
    else
    {
        dx=cos(atan2(toX-x, toY-y))*dist;
        dy=sin(atan2(toX-x, toY-y))*dist;
    }
    int dHeight=0;
    if(toY-y!=0 && toX-x!=0)
    {
        double slope=(toY-y)/(toX-x);
        if(abs(dx)>=abs(dy)) //tends to horizontal rather than vertical
        {
            int cx=abs(dx)/dx;
            int sum=0;
            int count=0;
            for(int i=0; i<=abs(dx); i++)
            {
                sum+=map[slope*(i*cx)+y][(i*cx)+x].height;
                count++;
            }
            dHeight=sum/count;
        }
        else
        {
            int cy=abs(dy)/dy;
            int sum=0;
            int count=0;
            for(int i=0; i<=abs(dy); i++)
            {
                sum+=map[(i*cy)+y][((i*cy)/slope)+x].height;
                count++;
            }
            dHeight=sum/count;
        }
    }
    else if(x==toX) //vertical
    {
        int sum=0;
        int cy=abs(dy)/dy;
        for(int i=0; i<=abs(dy); i++)
            sum+=map[cy*i+y][x].height;
        dHeight=sum/abs(dy);
    }
    else if(y==toY) //horizontal
    {
        int sum=0;
        int cx=abs(dx)/dx;
        for(int i=0; i<=abs(dx); i++)
            sum+=map[y][cx*i+x].height;
        dHeight=sum/abs(dx);
    }
    dHeight=map[y][x].height-dHeight;
    //the decrease in speed is applied after traveling that distance but before reaching the target. Realistically, speed would decrease real-time, and calculus would probably be involved. Oh well. Screw that. This is a discrete universe, not a continuous one. 
    speed+=dHeight*Throwing::HeightDiffSpeedChange;
    speed-=weight*Throwing::SpeedDecPerWeight;
    if(speed<=0)
        speed=0;
    map[y+dy][x+dx].allObjects.push_back(*this);
    map[y][x].allObjects.erase(map[y][x].allObjects.begin()+index);
    y+=dy;
    x+=dx;
    index=map[y][x].allObjects.size()-1;
    if(y==toY && x==toX) //I hit my target
    {
        if(map[y][x].uniton)
            allUnits.data[map[y][x].unitplayer][map[y][x].unitindex].hitWithFlyingObject(index);
        speed=0;
    }
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