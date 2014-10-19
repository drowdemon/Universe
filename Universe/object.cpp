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

object::object(short w, short p, short i, short px, short py, short what, bool aedib, food pf, short ind, short h)
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
    height=h;
}

object::object(objectDescriptor& od, short p, short i, short px, short py, short ind, short h)
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
    speed=0;
    toX=toY=-1;
    index=ind;
    height=h;
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
    if(dist==0)
        return;
    double dx=0;
    double dy=0;
    if(sqrt(pow(x-toX,2)+pow(y-toY,2))<=dist)
    {
        dx=toX-x;
        dy=toY-y;
    }
    else
    {
        dx=(double)cos(atan2((double)(toY-y), (double)(toX-x)))*(double)dist;
        dy=(double)sin(atan2((double)(toY-y), (double)(toX-x)))*(double)dist;
    }
    int dHeight=0;
    if(abs(toY-y)>0.0005 && abs(toX-x)>0.0005) //nonzero
    {
        double slope=(toY-y)/(toX-x);
        if(abs(dx)>=abs(dy)) //tends to horizontal rather than vertical
        {
            int cx=(int)(abs(dx)/dx);
            int sum=0;
            int count=0;
            for(double i=0; i<=abs(dx); i+=.1)
            {
                sum+=map[(unsigned int)(slope*(i*cx)+y)][(unsigned int)((i*cx)+x)].height;
                count++;
            }
            dHeight=sum/count;
        }
        else
        {
            int cy=(int)(abs(dy)/dy);
            int sum=0;
            int count=0;
            for(double i=0; i<=abs(dy); i+=.1)
            {
                sum+=map[(unsigned int)((i*cy)+y)][(unsigned int)(((i*cy)/slope)+x)].height;
                count++;
            }
            dHeight=sum/count;
        }
    }
    else if(abs(toX-x)<0.0005) //vertical
    {
        int sum=0;
        int cy=(int)(abs(dy)/dy);
        int count;
        for(double i=0; i<=abs(dy); i+=.1)
        {
            sum+=map[(unsigned int)(cy*i+y)][(unsigned int)x].height;
            count++;
        }
        dHeight=sum/count;
    }
    else if(abs(toY-y)<0.0005) //horizontal
    {
        int sum=0;
        int cx=(int)(abs(dx)/dx);
        int count=0;
        for(double i=0; i<=abs(dx); i+=.1)
        {
            sum+=map[(unsigned int)y][(unsigned int)(cx*i+x)].height;
            count++;
        }
        dHeight=sum/count;
    }
    dHeight=map[(unsigned int)y][(unsigned int)x].height-dHeight;
    //the decrease in speed is applied after traveling that distance but before reaching the target. Realistically, speed would decrease real-time, and calculus would probably be involved. Oh well. Screw that. This is a discrete universe, not a continuous one. 
    speed+=(dHeight*Throwing::HeightDiffSpeedChange);
    speed-=(weight*Throwing::SpeedDecPerWeight);
    if(speed<=0)
    {
        speed=0;
        toX=-1;
        toY=-1;
    }
    
    y+=dy;
    x+=dx;
    if(abs(toY-y)<0.0005 && abs(toX-x)<0.0005) //I hit my target
    {
        y=toY; //make it the exact int
        x=toX;
        if(y<toY)
            y+=0.000005; //if its slightly less than the int, increase it 
        if(x<toX)        
            x+=0.000005;
    }
    index=map[(unsigned int)y][(unsigned int)x].allObjects.size();
    
    map[(unsigned int)(y)][(unsigned int)(x)].allObjects.push_back(this);
    map[(unsigned int)(y-dy+0.000005)][(unsigned int)(x-dx+0.000005)].allObjects.erase(map[(unsigned int)(y-dy+0.000005)][(unsigned int)(x-dx+0.000005)].allObjects.begin()+index);
    if(abs(toY-y)<0.0005 && abs(toX-x)<0.0005) //I hit my target
    {
        if(map[(unsigned int)y][(unsigned int)x].uniton)
            allUnits.data[map[(unsigned int)y][(unsigned int)x].unitplayer][map[(unsigned int)y][(unsigned int)x].unitindex]->hitWithFlyingObject(index);
        map[(unsigned int)(y)][(unsigned int)(x)].allObjects.back()->speed=0;
        map[(unsigned int)(y)][(unsigned int)(x)].allObjects.back()->toX=-1;
        map[(unsigned int)(y)][(unsigned int)(x)].allObjects.back()->toY=-1;
    }
}

#define Z(type, val) \
    type object::get ## val(unit *u) \
    { \
        if(u->player==curLoops.unitPlayer && u->index==curLoops.unitPlayer && abs(u->y-y) > u->lineOfSight && abs(u->x-x) > u->lineOfSight) \
        { \
            if((heldByPlayer==u->player && heldByIndex==u->index) || ((*u->currSeen)[u->lineOfSight+y-u->y][u->lineOfSight+x-u->x].get(u)>0)) \
            { \
                if(height>=map[y][x].height) \
                    return val; \
            } \
        } \
        return -9999; \
    } 
    LISTVARSOBJVAR
#undef Z

#define Z(type, val) \
    type object::get ## val(unit *u) \
    { \
        if(u->player==curLoops.unitPlayer && u->index==curLoops.unitPlayer && abs(u->y-y) > u->lineOfSight && abs(u->x-x) > u->lineOfSight) \
        { \
            if((heldByPlayer==u->player && heldByIndex==u->index) || ((*u->currSeen)[u->lineOfSight+y-u->y][u->lineOfSight+x-u->x].get(u)>0)) \
            { \
                if(height>=map[y][x].height) \
                    return allObjectDesc[whatIsIt].val; \
            } \
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
            if(wy==y && wx==x && abs(wy-h->centery)<h->range && abs(wx-h->centerx)<h->range && mapseenhive[h->player][h->index][y][x].get(h)>0 && height>=map[y][x].height) \
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
            if(wx==x && wy==y && abs(wy-h->centery)<h->range && abs(wx-h->centerx)<h->range && mapseenhive[h->player][h->index][y][x].get(h)>0 && height>=map[y][x].height) \
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
        if(h->player==curLoops.hivePlayer && h->index==curLoops.hivePlayer && h->player==heldByPlayer && unitIndex==heldByIndex) \
        { \
            if(allUnits.data[h->player][unitIndex]) \
            { \
                if(abs(allUnits.data[h->player][unitIndex]->y - h->centery)<h->range && abs(allUnits.data[h->player][unitIndex]->x - h->centerx)<h->range && height>=map[y][x].height && mapseenhive[h->player][h->index][y][x].get(h)==1) \
                { \
                    return val; \
                } \
            } \
        } \
        return -9999; \
    } 
    LISTVARSOBJVAR
#undef Z

#define Z(type, val) \
    type object::get ## val(hiveMind *h, short unitIndex) \
    { \
        if(h->player==curLoops.hivePlayer && h->index==curLoops.hivePlayer && h->player==heldByPlayer && unitIndex==heldByIndex) \
        { \
            if(allUnits.data[h->player][unitIndex]) \
            { \
                if(abs(allUnits.data[h->player][unitIndex]->y - h->centery)<h->range && abs(allUnits.data[h->player][unitIndex]->x - h->centerx)<h->range && height>=map[y][x].height && mapseenhive[h->player][h->index][y][x].get(h)==1) \
                { \
                    return allObjectDesc[whatIsIt].val; \
                } \
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
