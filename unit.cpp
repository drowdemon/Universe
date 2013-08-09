#include <cstdlib>
#include "unit.h"
#include "globals.h"
#include "disease.h"
#include "dataStructures.h"

unit::unit(int p, int i, short s, short e, short str, bool g, short intel, unsigned char a, short px, short py, short pspeed, short los, short immun)
{
    player=p;
    index=i;
    sleep=s;
    energy=e;
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
}
bool unit::nextFrame()
{
    livingCosts();
    if(!checkLive())
        return false;
    move();
    if(!checkLive())
        return false;
    infect();
    diseaseEffects();
    if(!checkLive())
        return false;
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
                if(map[y+1][x+1].walkable(map[y][x].height,x,y))
                {   
                    if(map[y][x].height-map[y+1][x+1].height>1) //possibly painful height differential
                        health-=(int)((double)(map[y][x].height-map[y+1][x+1].height-1)*(double)FALLINGMULTIPLIER);
                    x++;
                    y++;
                    energy-=MOVEMENTENERGY;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
            else if(moveToY==y)
            {
                if(map[y][x+1].walkable(map[y][x].height,x,y))
                {   
                    if(map[y][x].height-map[y][x+1].height>1) //possibly painful height differential
                        health-=(int)((double)(map[y][x].height-map[y][x+1].height-1)*(double)FALLINGMULTIPLIER);
                    x++;
                    energy-=MOVEMENTENERGY;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
            else if(moveToY<y)
            {
                if(map[y-1][x+1].walkable(map[y][x].height,x,y))
                {   
                    if(map[y][x].height-map[y-1][x+1].height>1) //possibly painful height differential
                        health-=(int)((double)(map[y][x].height-map[y-1][x+1].height-1)*(double)FALLINGMULTIPLIER);
                    x++;
                    y--;
                    energy-=MOVEMENTENERGY;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
        }
        else if(moveToX==x)
        {
            if(moveToY>y)
            {
                if(map[y+1][x].walkable(map[y][x].height,x,y))
                {   
                    if(map[y][x].height-map[y+1][x].height>1) //possibly painful height differential
                        health-=(int)((double)(map[y][x].height-map[y+1][x].height-1)*(double)FALLINGMULTIPLIER);
                    y++;
                    energy-=MOVEMENTENERGY;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
            else if(moveToY<y)
            {
                if(map[y-1][x].walkable(map[y][x].height,x,y))
                {   
                    if(map[y][x].height-map[y-1][x].height>1) //possibly painful height differential
                        health-=(int)((double)(map[y][x].height-map[y-1][x].height-1)*(double)FALLINGMULTIPLIER);
                    y--;
                    energy-=MOVEMENTENERGY;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
        }
        if(moveToX<x)
        {
            if(moveToY>y)
            {
                if(map[y+1][x-1].walkable(map[y][x].height,x,y))
                {   
                    if(map[y][x].height-map[y+1][x-1].height>1) //possibly painful height differential
                        health-=(int)((double)(map[y][x].height-map[y+1][x-1].height-1)*(double)FALLINGMULTIPLIER);
                    x--;
                    y++;
                    energy-=MOVEMENTENERGY;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
            else if(moveToY==y)
            {
                if(map[y][x-1].walkable(map[y][x].height,x,y))
                {   
                    if(map[y][x].height-map[y][x-1].height>1) //possibly painful height differential
                        health-=(int)((double)(map[y][x].height-map[y][x-1].height-1)*(double)FALLINGMULTIPLIER);
                    x--;
                    energy-=MOVEMENTENERGY;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
            else if(moveToY<y)
            {
                if(map[y-1][x-1].walkable(map[y][x].height,x,y))
                {   
                    if(map[y][x].height-map[y-1][x-1].height>1) //possibly painful height differential
                        health-=(int)((double)(map[y][x].height-map[y-1][x-1].height-1)*(double)FALLINGMULTIPLIER);
                    x--;
                    y--;
                    energy-=MOVEMENTENERGY;
                }
                else
                {
                    moveToX=x;
                    moveToY=y;
                }
            }
        }
    }
}
void unit::infect()
{
    if(frames%INFECTRATE==0)
    {
        for(int i=0; i<allDiseases.size(); i++) //try to auto-infect
        {
            if(allDiseases[i].first!=2) //can catch this disease at random
            {
                if(rand()%10000<allDiseases[i].firstChance-immunity) //got sick
                {
                    if(allDiseases[i].first==1)
                    {
                        allDiseases[i].first++;
                        diseased.push_back(diseaseInfo(i));
                    }
                }
            }
        }
        for(int h=0; h<diseased.size(); h++) //attempt to infect others
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
            if(rand()%10000<allDiseases[diseased[h]].curability+immunity)
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
                                for(int d=0; d<map[i][j].disease.size(); d++)
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
                                for(int d=0; d<map[i][j].disease.size(); d++)
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
                                if(rand()%10000<allDiseases[diseased[h]].spreadabilityChance-allUnits[map[i][j].unitplayer][map[i][j].unitindex].immunity)
                                {
                                    bool good=true;
                                    for(int d=0; d<allUnits[map[i][j].unitplayer][map[i][j].unitindex].diseased.size(); d++)
                                    {
                                        if(diseased[h]==allUnits[map[i][j].unitplayer][map[i][j].unitindex].diseased[d]) //you actually managed to get sick with the same disease TWICE! Good for you!
                                        {
                                            good=false;
                                            allUnits[map[i][j].unitplayer][map[i][j].unitindex].diseased[d].multiplier++; //makes the disease more fearsome. Or at least more advanced in development
                                            allUnits[map[i][j].unitplayer][map[i][j].unitindex].diseased[d].flipDir=false; //If you were getting better, screw that! Now you're getting sicker again!
                                            break;
                                        }
                                    }
                                    if(good)
                                        allUnits[map[i][j].unitplayer][map[i][j].unitindex].diseased.push_back(diseased[h]);
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
    for(int i=0; i<diseased.size(); i++)
        energy-=allDiseases[diseased[i]].energyCost;
}
void unit::livingCosts()
{
    energy-=LIVINGENERGY;
    sleep-=1;
}
bool unit::checkLive()
{
    if(health<0)
        return false;
    if(energy<0)
        return false;
    if(sleep<0)
        return false; //death by extreme sleep deprivation
    return true;
}