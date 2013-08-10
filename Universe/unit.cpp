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
}
bool unit::nextFrame()
{
    if(age==-1) //in womb
        return true; //complete successfully
    livingEvents();
    if(!checkLive())
        return false;
    move();
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
    return true;
}
void unit::move()
{
    if(moveToX==x && moveToY==y)
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
void unit::moveHelper(int mx, int my)
{
    if(map[y+my][x+mx].walkable(this))
    {   
        int damage=0;
        if(map[y][x].height-map[y+my][x+mx].height>1) //possibly painful height differential
            health-=(damage=(int)((double)(map[y][x].height-map[y+my][x+mx].height-1)*(double)FALLINGMULTIPLIER));
        for(unsigned int i=0; i<allMinds.data[player].size(); i++)
            unseehive(i);
        x++;
        y++;
        energy-=MOVEMENTENERGY;
        unitChangeLog::update(x-mx,y-y,player,index,1,1,-damage,MOVEMENTENERGY,0,0,0);
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
    unitChangeLog::update(x,y,player,index,0,0,0,(energy-start),0,0,0);
}
void unit::livingEvents()
{
    int deltaE=energy;
    int deltaH=hunger;
    int deltaP=pregnant;
    int deltaHlth=health;
    energy-=LIVINGENERGY;
    sleep-=1;
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
    unitChangeLog::update(x,y,player,index,0,0,health-deltaHlth,energy-deltaE,hunger-deltaH,-1,pregnant-deltaP); //plain -1 is sleep
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
bool unit::reproduce(int withwhom)
{
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
                allUnits.data[player].push_back(unit(player, allUnits.data.size(),geneMixer(strength,allUnits.data[player][withwhom].strength),(bool)(rand()%2),geneMixer(intelligence,allUnits.data[player][withwhom].intelligence),-1,-1,-1,speed,lineOfSight,geneMixer(immunity,allUnits.data[player][withwhom].immunity),geneMixer(healthDiseaseInc,allUnits.data[player][withwhom].healthDiseaseInc),geneMixer(woundEnergyCost,allUnits.data[player][withwhom].woundEnergyCost),geneMixer(energyPerFood,allUnits.data[player][withwhom].energyPerFood),geneMixer(metabolicRate,allUnits.data[player][withwhom].metabolicRate),geneMixer(maxMetabolicRate,allUnits.data[player][withwhom].maxMetabolicRate),geneMixer(sexuallyMature,allUnits.data[player][withwhom].sexuallyMature))); //adds the new unit. It doesn't really exist though
                energy-=REPRODUCTIONENERGYCOST;
                allUnits.get(this,withwhom)->energy-=REPRODUCTIONENERGYCOST;
                return true;
            }
        }
    }
    return false; //repro failed
}
void unit::giveBirth()
{
    pregnant=-1;
    health-=BIRTHHEALTHLOSS;
    unit* child=allUnits.get(this,fetusid);
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
    unitChangeLog::update(child->x,child->y,player,index,0,0,child->health,child->energy,child->hunger,child->sleep,child->pregnant);
}

//getters 

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