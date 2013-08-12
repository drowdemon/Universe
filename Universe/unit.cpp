#include <cstdlib>
#include "unit.h"
#include "globals.h"
#include "disease.h"
#include "dataStructures.h"
#include "hivemind.h"

unit::unit(int p, int i, short str, bool g, short intel, char a, short px, short py, short pspeed, short los, short immun, short hdi, short wec, short epi, short mr, short mmr, short sm)
{
    player=p;
    index=i;
    sleep=NEWBORNSLEEP;
    energy=NEWBORNENERGY;
    strength=str;
    gender=g;
    intelligence=intel;
    age=a;
    moveToX=x=px;
    moveToY=y=py;   
    speed=pspeed;
    lineOfSight=los;
    immunity=immun;
    health=MAXHEALTH;
    healthDiseaseInc=hdi;
    woundEnergyCost=wec;
    energyPerFood=epi;
    metabolicRate=mr;
    maxMetabolicRate=mmr;
    sexuallyMature=sm;
    pregnant=-1;
    fetusid=-1;
    hunger=NEWBORNHUNGER;
    sleeping=false;
}
bool unit::nextFrame()
{
    if(age==-1) //in womb
        return true; //complete successfully
    livingEvents();
    if(!checkLive())
        return false;
    move(); //remove this line
    for(unsigned int i=0; i<allMinds.data[player].size(); i++)
        seehive(i);
    seeunit();
    if(!checkLive())
    {
        unseeunit();
        for(unsigned int i=0; i<allMinds.data[player].size(); i++)
                unseehive(i);
        return false;
    }
    infect();
    diseaseEffects();
    if(!checkLive())
    {
        unseeunit();
        for(unsigned int i=0; i<allMinds.data[player].size(); i++)
                unseehive(i);
        return false;
    }
    act(); //AI
    if(!checkLive())
    {
        unseeunit();    
        for(unsigned int i=0; i<allMinds.data[player].size(); i++)
                unseehive(i);
        return false;
    }
    unseeunit();
    emergencySleep();
    return true;
}
void unit::move()
{
    if(moveToX==x && moveToY==y)
        return;
    if(index!=curLoops.unitIndex || player!=curLoops.unitPlayer)
        return;
    if(frames%speed==0)
    {
        if(moveToX>x)
        {
            if(moveToY>y)
            {
                moveHelper(1,1);
            }
            else if(moveToY==y)
            {
                moveHelper(1,0);
            }
            else if(moveToY<y)
            {
                moveHelper(1,-1);
            }
        }
        else if(moveToX==x)
        {
            if(moveToY>y)
            {
                moveHelper(0,1);
            }
            else if(moveToY<y)
            {
                moveHelper(0,-1);
            }
        }
        if(moveToX<x)
        {
            if(moveToY>y)
            {
                moveHelper(-1,1);
            }
            else if(moveToY==y)
            {
                moveHelper(-1,0);
            }
            else if(moveToY<y)
            {
                moveHelper(-1,-1);
            }
        }
    }
}
void unit::move(short mx, short my)
{
    if(moveToX==x && moveToY==y)
        return;
    if(index!=curLoops.unitIndex || player!=curLoops.unitPlayer)
        return;
    if(frames%speed!=0)
        return;
    if(abs(x-mx)<=1 && abs(y-my)<=1)
        moveHelper(mx-x,my-y);
}
void unit::moveHelper(int mx, int my)
{
    if(map[y+my][x+mx].walkable(this))
    {   
        int damage=0;
        if(map[y][x].height-map[y+my][x+mx].height>1) //possibly painful height differential
            health-=(damage=(int)((double)(map[y][x].height-map[y+my][x+mx].height-1)*(double)FALLINGMULTIPLIER));
        for(unsigned int i=0; i<allMinds.data[player].size(); i++)
            unseehive(i);
        
        map[y][x].uniton=false;
        map[y][x].unitplayer=-1;
        map[y][x].unitindex=-1;
        x+=mx;
        y+=my;
        energy-=MOVEMENTENERGY;
        map[y][x].uniton=true;
        map[y][x].unitplayer=player;
        map[y][x].unitindex=index;
        unitChangeLog::update(x-mx,y-my,player,index,mx,my,-damage,MOVEMENTENERGY,0,0,0);
    }
    else
    {
        moveToX=x;
        moveToY=y;
    }
}
void unit::infect()
{
    int immunityloss=0;
    for(unsigned int i=0; i<diseased.size(); i++)
        immunityloss+=allDiseases[diseased[i]].immunCost;
    if(frames%INFECTRATE==0)
    {
        for(unsigned int i=0; i<allDiseases.size(); i++) //try to auto-infect
        {
            if(allDiseases[i].first!=2) //can catch this disease at random
            {
                if(rand()%10000<allDiseases[i].firstChance-((immunity-immunityloss>0)?(immunity-immunityloss):0)+((MAXHEALTH-health)*healthDiseaseInc)) //got sick
                {
                    if(allDiseases[i].first==1)
                    {
                        allDiseases[i].first++;
                        diseased.push_back(diseaseInfo(i));
                        strength-=allDiseases[i].permStrCost;
                        intelligence-=allDiseases[i].permIntelCost;
                        immunity-=allDiseases[i].permImmunCost;
                    }
                }
            }
        }
        for(unsigned int h=0; h<diseased.size(); h++) //attempt to infect others
        {
            diseased[h].time++;
            if(diseased[h].time%allDiseases[diseased[h]].multiplierRate==0)
            {
                if(diseased[h].multiplier<allDiseases[diseased[h]].multiplierMax && !diseased[h].flipDir)
                    diseased[h].multiplier++;
                else if(diseased[h].multiplier==allDiseases[diseased[h]].multiplierMax)
                    diseased[h].flipDir=true;
                else if(diseased[h].flipDir)
                    diseased[h].multiplier--;
                if(diseased[h].multiplier==0)
                {
                    diseased.erase(diseased.begin()+h); //cured
                    h--;
                }
            }
            if(rand()%10000<allDiseases[diseased[h]].curability+((immunity-immunityloss>0)?(immunity-immunityloss):0)-((MAXHEALTH-health)*healthDiseaseInc))
            {
                diseased.erase(diseased.begin()+h); //cured
                h--;
            }  
            if(diseased[h].time>allDiseases[diseased[h]].timeForSpreadability)
            {
                for(int i = (y>allDiseases[diseased[h]].spreadabilityArea) ? (y-allDiseases[diseased[h]].spreadabilityArea) : 0; i < (MAPSIZE-y<allDiseases[diseased[h]].spreadabilityArea) ? (y+allDiseases[diseased[h]].spreadabilityArea) : MAPSIZE; i++)
                {
                    for(int j = (x>allDiseases[diseased[h]].spreadabilityArea) ? (x-allDiseases[diseased[h]].spreadabilityArea) : 0; j < (MAPSIZE-x<allDiseases[diseased[h]].spreadabilityArea) ? (x+allDiseases[diseased[h]].spreadabilityArea) : MAPSIZE; j++)
                    {
                        if(map[i][j].water>0 && (allDiseases[diseased[h]].transmit&WATER_TRANSMIT)>0) //near water and can transmit through water
                        {
                            if(rand()%10000<allDiseases[diseased[h]].spreadabilityChance)
                            {
                                bool good=true;
                                for(unsigned int d=0; d<map[i][j].disease.size(); d++)
                                {
                                    if(map[i][j].disease[d]==diseased[h])
                                    {
                                        map[i][j].diseaseTime[d]=0;
                                        good=false;
                                        break;
                                    }
                                }
                                if(good)
                                {
                                    map[i][j].disease.push_back(diseased[h]);
                                    map[i][j].diseaseTime.push_back(0);
                                }
                            }
                        }
                        if(map[i][j].waste>0 && (allDiseases[diseased[h]].transmit&WASTE_TRANSMIT)>0) // near waste and can transmit
                        {
                            if(rand()%10000<allDiseases[diseased[h]].spreadabilityChance)
                            {
                                bool good=true;
                                for(unsigned int d=0; d<map[i][j].disease.size(); d++)
                                {
                                    if(map[i][j].disease[d]==diseased[h])
                                    {
                                        map[i][j].diseaseTime[d]=0;
                                        good=false;
                                        break;
                                    }
                                }
                                if(good)
                                {
                                    map[i][j].disease.push_back(diseased[h]);
                                    map[i][j].diseaseTime.push_back(0);
                                }
                            }
                        }
                        if(map[i][j].uniton && (allDiseases[diseased[h]].transmit&CONTACT_TRANSMIT)>0)
                        {
                            if(map[i][j].x!=x || map[i][j].y!=y) //different unit
                            {
                                int tempimmunloss=0;
                                for(unsigned int d=0; d<allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased.size(); d++)
                                    tempimmunloss+=allDiseases[allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased[d]].immunCost;
                                if(rand()%10000<allDiseases[diseased[h]].spreadabilityChance-((allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].immunity-tempimmunloss>0)?(allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].immunity-tempimmunloss):0)+((MAXHEALTH-allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].health)*allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].healthDiseaseInc))
                                {
                                    bool good=true;
                                    for(unsigned int d=0; d<allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased.size(); d++)
                                    {
                                        if(diseased[h]==allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased[d]) //you actually managed to get sick with the same disease TWICE! Good for you!
                                        {
                                            good=false;
                                            allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased[d].multiplier++; //makes the disease more fearsome. Or at least more advanced in development
                                            allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased[d].flipDir=false; //If you were getting better, screw that! Now you're getting sicker again!
                                            break;
                                        }
                                    }
                                    if(good)
                                    {
                                        allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased.push_back(diseased[h].disease);
                                        allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].strength-=allDiseases[diseased[h]].permStrCost;
                                        allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].intelligence-=allDiseases[diseased[h]].permIntelCost;
                                        allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].immunity-=allDiseases[diseased[h]].permImmunCost;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
void unit::diseaseEffects()
{
    int start=energy;
    for(unsigned int i=0; i<diseased.size(); i++)
        energy-=allDiseases[diseased[i]].energyCost;
    if(energy-start!=0)
        unitChangeLog::update(x,y,player,index,0,0,0,(energy-start),0,0,0);
}
void unit::livingEvents()
{
    int deltaE=energy;
    int deltaH=hunger;
    int deltaP=pregnant;
    int deltaS=sleep;
    int deltaHlth=health;
    energy-=LIVINGENERGY;
    if(sleeping)
        sleep+=3;
    else
        sleep--;
    energy-=(MAXHEALTH-health)*woundEnergyCost;
    if(energy<ENERGYCRITPOINT)
    {
        if(frames%maxMetabolicRate==0)
        {
            hunger++;
            energy+=energyPerFood;
        }
    }
    else
    {
        if(frames%metabolicRate==0)
        {
            hunger++;
            energy+=energyPerFood;
        }
    }
    if(pregnant>=0)
        pregnant++;
    if(pregnant>=GESTATIONPERIOD) //birth
    {
        giveBirth(); //changes health and pregnant
    }
    unitChangeLog::update(x,y,player,index,0,0,health-deltaHlth,energy-deltaE,hunger-deltaH,sleep-deltaS,pregnant-deltaP);
}
bool unit::checkLive()
{
    if(health<0) 
        return false; //death by physical damage. 
    if(energy<0)
        return false; //death by energy loss. Unlikely to happen unless you are diseased. This is how diseases kill.
    if(sleep<0)
        return false; //death by extreme sleep deprivation
    if(hunger>=MAXHUNGER) 
        return false; //death by starvation
    return true;
}
void unit::seeunit()
{
    for(int i=0; i<=lineOfSight; i++) //reveals map, in a square that is growing out from the central point. It does this so that later when I implement walls and stuff its easier to block things, since you'll be seeing in a nicer order.
    {
        for(int j=-i; j<=i; j++)
        {
            mapseenunit[player][y+i][x+j].b=true;
            mapseenunit[player][y-i][x+j].b=true;
            mapseenunit[player][y+j][x+i].b=true;
            mapseenunit[player][y+j][x-i].b=true;
        }
    }
}
void unit::unseeunit()
{
    for(int i=0; i<=lineOfSight; i++) //reveals map, in a square that is growing out from the central point. It does this so that later when I implement walls and stuff its easier to block things, since you'll be seeing in a nicer order.
    {
        for(int j=-i; j<=i; j++)
        {
            mapseenunit[player][y+i][x+j].b=false;
            mapseenunit[player][y-i][x+j].b=false;
            mapseenunit[player][y+j][x+i].b=false;
            mapseenunit[player][y+j][x-i].b=false;
        }
    }
}
void unit::seehive(int hiveindex)
{
    if(abs(allMinds.data[player][hiveindex].centerx-x)<allMinds.data[player][hiveindex].range && abs(allMinds.data[player][hiveindex].centery-y)<allMinds.data[player][hiveindex].range) //in range of hive
    {
        for(int i=0; i<=lineOfSight; i++) //reveals map, in a square that is growing out from the central point. It does this so that later when I implement walls and stuff its easier to block things, since you'll be seeing in a nicer order.
        {
            for(int j=-i; j<=i; j++)
            {
                mapseenhive[player][hiveindex][y+i][x+j].b=true;
                mapseenhive[player][hiveindex][y-i][x+j].b=true;
                mapseenhive[player][hiveindex][y+j][x+i].b=true;
                mapseenhive[player][hiveindex][y+j][x-i].b=true;
            }
        }
    }
}
void unit::unseehive(int hiveindex)
{
    if(abs(allMinds.data[player][hiveindex].centerx-x)<allMinds.data[player][hiveindex].range && abs(allMinds.data[player][hiveindex].centery-y)<allMinds.data[player][hiveindex].range) //in range of hive
    {
        for(int i=0; i<=lineOfSight; i++) //hides map, in a square that is growing out from the central point. It does this so that later when I implement walls and stuff its easier to block things, since you'll be seeing in a nicer order.
        {
            for(int j=-i; j<=i; j++) //now its in a square because it was before, and since there are no repeats its the same amount of work
            {
                mapseenhive[player][hiveindex][y+i][x+j].b=false;
                mapseenhive[player][hiveindex][y-i][x+j].b=false;
                mapseenhive[player][hiveindex][y+j][x+i].b=false;
                mapseenhive[player][hiveindex][y+j][x-i].b=false;
            }
        }
    }
}
void unit::act()
{
}
void unit::reproduce(int withwhom)
{
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(abs(x-allUnits.get(this,withwhom)->x)<=1 && abs(y-allUnits.get(this,withwhom)->y)<=1) //close enough
    {
        if(age>=sexuallyMature && allUnits.get(this,withwhom)->age>allUnits.get(this,withwhom)->sexuallyMature) //old enough
        {
            if(gender!=allUnits.get(this,withwhom)->gender) //different genders
            {
                if(!gender) //female
                {
                    pregnant=0;
                    fetusid=allUnits.data[player].size();
                    unitChangeLog::update(x,y,player,index,0,0,0,-REPRODUCTIONENERGYCOST,0,0,1);
                    unitChangeLog::update(allUnits.data[player][withwhom].x,allUnits.data[player][withwhom].y,player,withwhom,0,0,0,-REPRODUCTIONENERGYCOST,0,0,0);
                }
                else //partner is female
                {
                    allUnits.data[player][withwhom].pregnant=0;
                    allUnits.data[player][withwhom].fetusid=allUnits.data[player].size();
                    unitChangeLog::update(x,y,player,index,0,0,0,-REPRODUCTIONENERGYCOST,0,0,0);
                    unitChangeLog::update(allUnits.data[player][withwhom].x,allUnits.data[player][withwhom].y,player,withwhom,0,0,0,-REPRODUCTIONENERGYCOST,0,0,1);
                }
                allUnits.data[player].push_back(unit(player, allUnits.data.size(),geneMixer(strength,allUnits.data[player][withwhom].strength),(bool)(rand()%2),geneMixer(intelligence,allUnits.data[player][withwhom].intelligence),-1,-1,-1,(speed+allUnits.data[player][withwhom].speed)/2,(lineOfSight+allUnits.data[player][withwhom].lineOfSight)/2,geneMixer(immunity,allUnits.data[player][withwhom].immunity),geneMixer(healthDiseaseInc,allUnits.data[player][withwhom].healthDiseaseInc),geneMixer(woundEnergyCost,allUnits.data[player][withwhom].woundEnergyCost),geneMixer(energyPerFood,allUnits.data[player][withwhom].energyPerFood),geneMixer(metabolicRate,allUnits.data[player][withwhom].metabolicRate),geneMixer(maxMetabolicRate,allUnits.data[player][withwhom].maxMetabolicRate),(sexuallyMature+allUnits.data[player][withwhom].sexuallyMature)/2)); //adds the new unit. It doesn't really exist though
                if(allUnits.data[player][fetusid].maxMetabolicRate>allUnits.data[player][fetusid].metabolicRate)
                    allUnits.data[player][fetusid].maxMetabolicRate=allUnits.data[player][fetusid].metabolicRate-3;
                energy-=REPRODUCTIONENERGYCOST;
                allUnits.get(this,withwhom)->energy-=REPRODUCTIONENERGYCOST;
            }
        }
    }
}
void unit::giveBirth()
{
    pregnant=-1;
    health-=BIRTHHEALTHLOSS;
    energy-=BIRTHENERGYLOSS;
    unit* child=&allUnits.data[player][fetusid];
    child->age=0;
    if(map[y+1][x].walkable(this))
    {
        child->moveToX=child->x=x;
        child->moveToY=child->y=y+1;
    }
    else if(map[y-1][x].walkable(this))
    {
        child->moveToX=child->x=x;
        child->moveToY=child->y=y-1;
    }
    else if(map[y][x+1].walkable(this))
    {
        child->moveToX=child->x=x+1;
        child->moveToY=child->y=y;
    }
    else if(map[y][x-1].walkable(this))
    {
        child->moveToX=child->x=x-1;
        child->moveToY=child->y=y;
    }
    else if(map[y-1][x-1].walkable(this))
    {
        child->moveToX=child->x=x-1;
        child->moveToY=child->y=y-1;
    }
    else if(map[y-1][x+1].walkable(this))
    {
        child->moveToX=child->x=x+1;
        child->moveToY=child->y=y-1;
    }
    else if(map[y+1][x+1].walkable(this))
    {
        child->moveToX=child->x=x+1;
        child->moveToY=child->y=y+1;
    }
    else if(map[y+1][x-1].walkable(this))
    {
        child->moveToX=child->x=x-1;
        child->moveToY=child->y=y+1;
    }
    map[child->y][child->x].uniton=true;
    map[child->y][child->x].unitplayer=child->player;
    map[child->y][child->x].unitindex=child->index;
    unitChangeLog::update(child->x,child->y,child->player,child->index,0,0,child->health,child->energy,child->hunger,child->sleep,child->pregnant);
}
void unit::emergencySleep()
{
    if(sleep>EMERGENCYSLEEPLVL && sleep<EMERGENCYENDSLEEP)
        return; //not emergency
    for(unsigned int i=0; i<allMinds.data[0].size(); i++)
    {
        if(getHiveMindcenterx(i)!=-9999) //legit data - the hivemind is available
        {
            return; //a hive mind is in command. I can't sleep without commands
        }
    }
    //no hivemind nearby, must sleep
    if(sleep<=EMERGENCYSLEEPLVL) //must sleep
        goToSleep();
    else //must awaken, middle taken care of on top
        awaken();
}
void unit::goToSleep()
{
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(map[y][x].waste>0 || map[y][x].bush>0) //bad ground for sleeping
        return;
    for(unsigned int i=0; i<map[y][x].allObjects.size(); i++)
    {
        if(!map[y][x].allObjects[i].getsleepable(this))
            return;
    }
    sleeping=true;
}
void unit::awaken()
{
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    sleeping=false;
}
void unit::die()
{
    for(unsigned int i=index+1; i<allUnits.data[player].size(); i++)
    {
        allUnits.data[player][i].index--;
        map[allUnits.data[player][i].y][allUnits.data[player][i].x].unitindex--;
    }
    map[y][x].uniton=false;
    map[y][x].unitplayer=-1;
    map[y][x].unitindex=-1;
    allUnits.data[player].erase(allUnits.data[player].begin()+index);
    unitChangeLog::update(-99999,-99999,player,index,-99999,-99999,-99999,-99999,-99999,-99999,-99999);
}
void unit::pickUp(int what, int ox, int oy)
{
    if(abs(ox-x)>1 || abs(oy-y)>1) //too far
        return;
    if(index!=curLoops.unitIndex || player!=curLoops.unitPlayer)
        return;
    int carriedWeight=0;
    for(unsigned int i=0; i<carrying.size(); i++)
        carriedWeight+=carrying[i].weight;
    for(unsigned int i=0; i<map[oy][ox].allObjects.size(); i++)
    {
        if(map[oy][ox].allObjects[i].whatIsIt==what)
        {
            if(carriedWeight+map[oy][ox].allObjects[i].weight<=strength) //good
            {
                carrying.push_back(map[oy][ox].allObjects[i]);
                map[oy][ox].allObjects.erase(map[oy][ox].allObjects.begin()+i);
            }
            break;
        }
    }
}
void unit::putDown(int objIndex, int px, int py)
{
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(abs(x-px)>1 || abs(y-py)>1)
        return; //not allowed
    carrying[objIndex].x=px;
    carrying[objIndex].y=py;
    carrying[objIndex].heldByIndex=-1;
    carrying[objIndex].heldByPlayer=-1;
    map[y][x].allObjects.push_back(carrying[objIndex]);
    carrying.erase(carrying.begin()+objIndex);
}
//getters 

vector<object> unit::getcarrying()
{
    if(curLoops.unitPlayer==player && curLoops.unitIndex==index) \
    { 
        return carrying; \
    } 
    return vector<object>(); 
}
#define Y(type, val) \
    type unit::get ## val() \
    { \
        if(curLoops.unitPlayer==player && curLoops.unitIndex==index) \
        { \
            return val; \
        } \
        return -127; \
    }
    LISTVARSUNIT
#undef Y

//getters for hive mind

#define X(type, val) \
    type unit::getHiveMind ## val(int hiveIndex) \
    { \
        if(curLoops.unitPlayer==player && curLoops.unitIndex==index) \
        { \
            if(abs(allMinds.data[player][hiveIndex].centerx-x)<allMinds.data[player][hiveIndex].range && abs(allMinds.data[player][hiveIndex].centery-y)<allMinds.data[player][hiveIndex].range) \
                return allMinds.data[player][hiveIndex].val ; \
        } \
        return -9999; \
    } 
    LISTVARSHIVE
#undef X