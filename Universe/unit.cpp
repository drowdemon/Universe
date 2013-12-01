#include <cstdlib>
#include "unit.h"
#include "globals.h"
#include "disease.h"
#include "dataStructures.h"
#include "hivemind.h"
#include "food.h"
#include "throw.h"
#include <iostream>

using namespace std;

unit::unit() : throwSkill(0)
{
}

unit::unit(int p, int i, short str, bool g, short intel, char a, short px, short py, short pspeed, short los, short immun, short hdi, short wec, short epi, short mr, short mmr, short sm, short throwXP, short wt, short ftw, short fre, short enm) : throwSkill(throwXP)
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
    learningSkills=new short[NUMSKILLS]; //update every time there is a new skill
    for(int i=0; i<NUMSKILLS; i++)
        learningSkills[i]=-1;
    
    sleeping=false;
    reproducing=0;
    moving=false;  
    throwing=false;
    eating=false;
    liftingOrDropping=false;
    waking=false;
    weight=wt;
    fatBuildProgress=0;
    fatToWeight=ftw;
    fatRetrievalEfficiency=fre;
    minWeight=NEWBORNMINWEIGHT;
    excreteNeed=-1;
    excreteNeedMax=enm;
    excreting=false;
}
unit::~unit()
{
    for(unsigned int i=0; i<carrying.size(); i++)
    {
        delete carrying[i];
        carrying[i]=NULL;
    }
    carrying.clear();
}
bool unit::nextFrame()
{
    resetSkills();
    if(age==-1) //in womb
        return true; //complete successfully
    livingEvents();
    if(!checkLive())
        return false;   
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
    learn(); //learn if you set that you want to and you're near whoever you are learning from and they did something
    unseeunit();
    emergencySleep();
    resetActions();
    return true;
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
        energy-=weight/50*MOVINGSELFWEIGHTPENALTY;
        energy-=((rand()%50)<(weight%50)) ? MOVINGSELFWEIGHTPENALTY : 0;
        int sumcarried=0;
        for(unsigned int i=0; i<carrying.size(); i++)
            sumcarried+=carrying[i]->weight;
        energy-=sumcarried/50*MOVINGLIFTEDWEIGHTPENALTY;
        energy-=((rand()%50)<(sumcarried%50)) ? MOVINGLIFTEDWEIGHTPENALTY : 0;
        map[y][x].uniton=true;
        map[y][x].unitplayer=player;
        map[y][x].unitindex=index;
        for(unsigned int i=0; i<allMinds.data[player].size(); i++)
            seehive(i);
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
    if(frames%INFECTRATE==0)
    {
        int immunityloss=0;
        for(unsigned int i=0; i<diseased.size(); i++)
            immunityloss+=allDiseases[diseased[i]].immunCost;
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
                        for(unsigned int k=0; k<map[i][j].allObjects.size(); k++)
                        {
                            if(map[i][j].allObjects[k]->actuallyEdible!=-3 && (allDiseases[diseased[h]].transmit&FOOD_TRANSMIT)>0)
                            {
                                if(rand()%10000<allDiseases[diseased[h]].spreadabilityChance)
                                {
                                    bool good=true;
                                    for(unsigned int d=0; d<map[i][j].allObjects[k]->infected.size(); d++)
                                    {
                                        if(map[i][j].allObjects[k]->infected[d]==diseased[h])
                                        {
                                            map[i][j].allObjects[k]->infectionTime[d]=0;
                                            good=false;
                                            break;
                                        }
                                    }
                                    if(good)
                                    {
                                        map[i][j].allObjects[k]->infected.push_back(diseased[h]);
                                        map[i][j].allObjects[k]->infectionTime.push_back(0);
                                    }
                                }
                            }
                        }
                        if(map[i][j].uniton && (allDiseases[diseased[h]].transmit&CONTACT_TRANSMIT)>0)
                        {
                            if(map[i][j].x!=x || map[i][j].y!=y) //different unit
                            {
                                int tempimmunloss=0;
                                for(unsigned int d=0; d<allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased.size(); d++)
                                    tempimmunloss+=allDiseases[allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased[d]].immunCost;
                                if(rand()%10000<allDiseases[diseased[h]].spreadabilityChance-((allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->immunity-tempimmunloss>0)?(allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->immunity-tempimmunloss):0)+((MAXHEALTH-allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->health)*allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->healthDiseaseInc))
                                {
                                    bool good=true;
                                    for(unsigned int d=0; d<allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased.size(); d++)
                                    {
                                        if(diseased[h]==allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased[d]) //you actually managed to get sick with the same disease TWICE! Good for you!
                                        {
                                            good=false;
                                            allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased[d].multiplier++; //makes the disease more fearsome. Or at least more advanced in development
                                            allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased[d].flipDir=false; //If you were getting better, screw that! Now you're getting sicker again!
                                            break;
                                        }
                                    }
                                    if(good)
                                    {
                                        allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased.push_back(diseased[h].disease);
                                        allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->strength-=allDiseases[diseased[h]].permStrCost;
                                        allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->intelligence-=allDiseases[diseased[h]].permIntelCost;
                                        allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->immunity-=allDiseases[diseased[h]].permImmunCost;
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
    energy-=LIVINGENERGYPERSTRENGTH*strength;
    energy-= ((rand()%5)<(strength%5)) ? LIVINGENERGYPERSTRENGTH : 0;
    if(sleeping)
        sleep+=3;
    else
        sleep--;
    energy-=(MAXHEALTH-health)*woundEnergyCost;
    if(energy<ENERGYFROMFATPOINT)
    {
        if(weight>NEWBORNMINWEIGHT && frames%ENERGYFROMFATRATE==0)
        {
            weight--;
            energy+=fatToWeight*fatRetrievalEfficiency/1000;
        }
    }
    if(energy<ENERGYCRITPOINT)
    {
        if(frames%maxMetabolicRate==0)
        {
            hunger++;
            energy+=energyPerFood;
            if(hunger%EXCRETIONFREQ==0 && hunger!=0)
                excreteNeed=0;
        }
    }
    else
    {
        if(frames%metabolicRate==0)
        {
            hunger++;
            energy+=energyPerFood;
            if(hunger%EXCRETIONFREQ==0 && hunger!=0)
                excreteNeed=0;
        }
    }
    if(pregnant>=0)
    {
        pregnant++;
        energy-=PREGNANTENERGYCOST*(2*(pregnant/1000));
    }
    if(reproducing>0)
    {
        reproducing++;
        if(pregnant>0)
            pregnant--; //not yet
        if(reproducing>REPRODUCTIONTIME)
            reproducing=0; //done
    }
    if(pregnant>=GESTATIONPERIOD) //birth
    {
        giveBirth(); //changes health and pregnant
    }
    if(energy>ENERGYSOFTMAX)
    {
        energy--;
        fatBuildProgress++;
        if(fatBuildProgress>=fatToWeight)
        {
            fatBuildProgress=0;
            weight++; 
        }
    }
    if(excreteNeed>=0)
    {
        excreteNeed++;
        if(excreteNeed>=excreteNeedMax)
            shit();
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
    vector<visionObstacle> obstacles; //stores slope to obstacles
    mapseenunit[player][y][x].b=1; //own square is completely visable
    for(int i=1; i<=lineOfSight; i++) //reveals map, in a square that is growing out from the central point. It does this so that later when I implement walls and stuff its easier to block things, since you'll be seeing in a nicer order.
    {
        for(int j=-i; j<=i; j++)
        {
            bool allowed[4]={true,true,true,true};
            double curSlopes[4]={(double)i/(double)j,(double)(-i)/(double)j,(double)j/(double)i,(double)j/(double)(-i)};
            point curPoints[4]={point(j,i),point(j,-i),point(i,j),point(-i,j)};
            for(unsigned int k=0; k<obstacles.size(); k++)
            {
                for(int h=0; h<4; h++)
                {
                    if(allowed[h]==false)
                        continue;
                    if(curSlopes[h]!=1.0/0.0) //slope is normal
                    {
                        if(obstacles[k].slope1<obstacles[k].slope2) //Q I or Q II   //Also includes all times when obstacles is on an axis
                        {
                            if((curPoints[h].y>0) || (obstacles[k].y==0 || obstacles[k].x==0)) //correct quadrant, or on axis
                            {
                                if(obstacles[k].slope2<curSlopes[h] && curSlopes[h]<obstacles[k].slope1) //between two slopes of obstacle: unseen
                                {
                                    if((obstacles[k].y==0 && ((curPoints[h].x<0)==(obstacles[k].x<0))) || (obstacles[k].x==0 && ((curPoints[h].y<0)==(obstacles[k].y<0)))) //extra test for the points on an axis
                                        allowed[h]=false;
                                }
                            }
                        }
                        else //Q III,Q IV //s2<s1.
                        {
                            if(curPoints[h].y<0)
                            {
                                if(obstacles[k].slope1<curSlopes[h] && curSlopes[h]<obstacles[k].slope2)
                                    allowed[h]=false;
                            }
                        }
                    }
                    else
                    {
                        if(abs(curPoints[h].y)>abs(obstacles[k].y) && ((curPoints[h].y<0)==(curPoints[h].y<0))) //point is greater than obstacle and has the same sign as the obstacle
                            allowed[h]=false;
                    }
                }
            }
            bool *ret;
            for(int h=0; h<4; h++) //for all 4 points I'm checking
            {
                if(allowed[h]) //if its not behind an obstacle
                {
                    if((( ((map[y+curPoints[h].y][x+curPoints[h].x].bush>=125) ? 124 : map[y+curPoints[h].y][x+curPoints[h].x].bush) / 25 * CAMEOPER25BUSH) + (((map[y+curPoints[h].y][x+curPoints[h].x].tree>0)?1:0) * CAMEOFORTREE) + (((map[y+curPoints[h].y][x+curPoints[h].x].road>0)?1:0) * CAMEOFORROAD)) > (lineOfSight-((i>abs(j))?i:abs(j)))) //if the sum of the various cameo affects makes whatever unit is on that square invisible, make sure that happens
                        mapseenunit[player][y+curPoints[h].y][x+curPoints[h].x].b=2;  //can see tile but not units on it
                    else
                        mapseenunit[player][y+curPoints[h].y][x+curPoints[h].x].b=1;  //can see tile and units on it                  
                    if((ret = map[y+curPoints[h].y][x+curPoints[h].x].blocksVision(this))) //if I saw an obstacle, add it to the list of obstacles
                        obstacles.push_back(visionObstacle(curPoints[h].x,curPoints[h].y));
                    delete ret; //delete the pointer. No memory leaks.
                }
            }
            /*if((( ((map[y+i][x+j].bush>=125) ? 124 : map[y+i][x+j].bush) / 25 * CAMEOPER25BUSH) + (((map[y+i][x+j].tree>0)?1:0) * CAMEOFORTREE) + (((map[y+i][x+j].road>0)?1:0) * CAMEOFORROAD)) > (lineOfSight-((i>abs(j))?i:abs(j)))) //if the sum of the various cameo affects makes whatever unit is on that square invisible, make sure that happens
                mapseenunit[player][y+i][x+j].b=2;
            else
                mapseenunit[player][y+i][x+j].b=1;
            
            if((( ((map[y-i][x+j].bush>=125) ? 124 : map[y-i][x+j].bush) / 25 * CAMEOPER25BUSH) + (((map[y-i][x+j].tree>0)?1:0) * CAMEOFORTREE) + (((map[y-i][x+j].road>0)?1:0) * CAMEOFORROAD)) > (lineOfSight-((i>j)?i:j)))
                mapseenunit[player][y-i][x+j].b=2;
            else
                mapseenunit[player][y-i][x+j].b=1;
            
            if((( ((map[y+j][x+i].bush>=125) ? 124 : map[y+j][x+i].bush) / 25 * CAMEOPER25BUSH) + (((map[y+j][x+i].tree>0)?1:0) * CAMEOFORTREE) + (((map[y+j][x+i].road>0)?1:0) * CAMEOFORROAD)) > (lineOfSight-((i>j)?i:j)))
                mapseenunit[player][y+j][x+i].b=2;
            else
                mapseenunit[player][y+j][x+i].b=1;
            
            if((( ((map[y+j][x-i].bush>=125) ? 124 : map[y+j][x-i].bush) / 25 * CAMEOPER25BUSH) + (((map[y+j][x-i].tree>0)?1:0) * CAMEOFORTREE) + (((map[y+j][x-i].road>0)?1:0) * CAMEOFORROAD)) > (lineOfSight-((i>j)?i:j)))
                mapseenunit[player][y+j][x-i].b=2;
            else
                mapseenunit[player][y+j][x-i].b=1;*/
            
            //if I just saw an obstacle, add it to the list of obstacles
            /*bool* rets[4];
            if((rets[0]=map[y+i][x+j].blocksVision(this)))
                obstacles.push_back(visionObstacle(j,i));
            if((rets[1]=map[y-i][x+j].blocksVision(this)))
                obstacles.push_back(visionObstacle(j,-i));
            if((rets[2]=map[y+j][x+i].blocksVision(this)))
                obstacles.push_back(visionObstacle(i,j));
            if((rets[3]=map[y+j][x-i].blocksVision(this)))
                obstacles.push_back(visionObstacle(-i,j));
            for(int i=0; i<4; i++)
                delete rets[i];*/
        }
    }
}
void unit::unseeunit()
{
    for(int i=0; i<=lineOfSight; i++) //reveals map, in a square that is growing out from the central point. It does this so that later when I implement walls and stuff its easier to block things, since you'll be seeing in a nicer order.
    {
        for(int j=-i; j<=i; j++)
        {
            mapseenunit[player][y+i][x+j].b=0;
            mapseenunit[player][y-i][x+j].b=0;
            mapseenunit[player][y+j][x+i].b=0;
            mapseenunit[player][y+j][x-i].b=0;
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
                if((( ((map[y+i][x+j].bush>=125) ? 124 : map[y+i][x+j].bush) / 25 * CAMEOPER25BUSH) + (((map[y+i][x+j].tree>0)?1:0) * CAMEOFORTREE) + (((map[y+i][x+j].road>0)?1:0) * CAMEOFORROAD)) > (lineOfSight-((i>abs(j))?i:abs(j)))) //if the sum of the various cameo affects makes whatever unit is on that square invisible, make sure that happens
                    mapseenhive[player][hiveindex][y+i][x+j].b=2;
                else
                    mapseenhive[player][hiveindex][y+i][x+j].b=1;

                if((( ((map[y-i][x+j].bush>=125) ? 124 : map[y-i][x+j].bush) / 25 * CAMEOPER25BUSH) + (((map[y-i][x+j].tree>0)?1:0) * CAMEOFORTREE) + (((map[y-i][x+j].road>0)?1:0) * CAMEOFORROAD)) > (lineOfSight-((i>j)?i:j)))
                    mapseenhive[player][hiveindex][y-i][x+j].b=2;
                else
                    mapseenhive[player][hiveindex][y-i][x+j].b=1;

                if((( ((map[y+j][x+i].bush>=125) ? 124 : map[y+j][x+i].bush) / 25 * CAMEOPER25BUSH) + (((map[y+j][x+i].tree>0)?1:0) * CAMEOFORTREE) + (((map[y+j][x+i].road>0)?1:0) * CAMEOFORROAD)) > (lineOfSight-((i>j)?i:j)))
                    mapseenhive[player][hiveindex][y+j][x+i].b=2;
                else
                    mapseenhive[player][hiveindex][y+j][x+i].b=1;

                if((( ((map[y+j][x-i].bush>=125) ? 124 : map[y+j][x-i].bush) / 25 * CAMEOPER25BUSH) + (((map[y+j][x-i].tree>0)?1:0) * CAMEOFORTREE) + (((map[y+j][x-i].road>0)?1:0) * CAMEOFORROAD)) > (lineOfSight-((i>j)?i:j)))
                    mapseenhive[player][hiveindex][y+j][x-i].b=2;
                else
                    mapseenhive[player][hiveindex][y+j][x-i].b=1;
                /*mapseenhive[player][hiveindex][y+i][x+j].b=true;
                mapseenhive[player][hiveindex][y-i][x+j].b=true;
                mapseenhive[player][hiveindex][y+j][x+i].b=true;
                mapseenhive[player][hiveindex][y+j][x-i].b=true;*/
            }
        }
    }
}
void unit::unseehive(int hiveindex)
{
    //if(abs(allMinds.data[player][hiveindex].centerx-x)<allMinds.data[player][hiveindex].range && abs(allMinds.data[player][hiveindex].centery-y)<allMinds.data[player][hiveindex].range) //in range of hive
    //{
        for(int i=0; i<=lineOfSight; i++) //hides map, in a square that is growing out from the central point. It does this so that later when I implement walls and stuff its easier to block things, since you'll be seeing in a nicer order.
        {
            for(int j=-i; j<=i; j++) //now its in a square because it was before, and since there are no repeats its the same amount of work
            {
                mapseenhive[player][hiveindex][y+i][x+j].b=0;
                mapseenhive[player][hiveindex][y-i][x+j].b=0;
                mapseenhive[player][hiveindex][y+j][x+i].b=0;
                mapseenhive[player][hiveindex][y+j][x-i].b=0;
            }
        }
    //}
}
void unit::act() //make this empty
{
    //move(); //remove this line. TESTING ONLY!!!
    if(index==0 && frames==1)
        throwObj(0,x+1,y+1,strength);
}
void unit::giveBirth()
{
    pregnant=-1;
    health-=BIRTHHEALTHLOSS;
    energy-=BIRTHENERGYLOSS;
    unit* child=allUnits.data[player][fetusid];
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
void unit::die()
{
    /*for(unsigned int i=index+1; i<allUnits.data[player].size(); i++)
    {
        allUnits.data[player][i]->index--;
        map[allUnits.data[player][i]->y][allUnits.data[player][i]->x].unitindex--;
    }*/
    map[y][x].uniton=false;
    map[y][x].unitplayer=-1;
    map[y][x].unitindex=-1;
    for(unsigned int i=0; i<carrying.size(); i++) //drop everything
    {
        carrying[i]->x=x;
        carrying[i]->y=y;
        carrying[i]->heldByIndex=-1;
        carrying[i]->heldByPlayer=-1;
        carrying[i]->index=map[y][x].allObjects.size();
        map[y][x].allObjects.push_back(carrying[i]);
    }
    map[y][x].allObjects.push_back(new object(allObjectDesc[OBJECT_CORPSE],-1,-1,x,y,map[y][x].allObjects.size(),map[y][x].height));
    map[y][x].allObjects.back()->weight=weight;
    
    if(fetusid!=-1) //if carrying a child in womb
    {
        int p=player;
        int fi=fetusid;
        delete allUnits.data[player][fetusid]; //kill it too. Map modifications are not required since the fetus is not technically on the map.
        allUnits.data[p][fi]=NULL;
    }
    //allUnits.data[player].erase(allUnits.data[player].begin()+index);
    int p=player;
    int i=index;
    delete allUnits.data[player][index];
    allUnits.data[p][i]=NULL;
    unitChangeLog::update(-99999,-99999,player,index,-99999,-99999,-99999,-99999,-99999,-99999,-99999);
}
void unit::hitWithFlyingObject(int objIndex) //add more factors to the damage. Object sharpness maybe. How hard/soft it is. 
{
    health-=map[y][x].allObjects[objIndex]->speed*SPEEDTODAMAGE; 
}
void unit::resetActions() //sleeping is controlled differently, and reproduction is in livingEvents()
{
    moving=false;
    throwing=false;
    eating=false;
    liftingOrDropping=false;
    waking=false;
}
void unit::resetSkills()
{
    throwSkill.threw=false;
}
void unit::learn()
{
    if(sleeping || reproducing>0 || throwing || liftingOrDropping || waking || excreting) //eating and moving are ok
        return; 
    for(int i=0; i<NUMSKILLS; i++)
    {
        if(learningSkills[i]>=0)
        {
            switch(i) //add more cases as skills are added.
            {
                case 0:
                    if(allUnits.data[player][learningSkills[i]] && throwSkill.learn(this, allUnits.data[player][learningSkills[i]])) //checks that teacher is not a null pointer //learn. If you learned, you can't learn anything else this cycle. You were paying attention to the guy doing this particular activity, screw the rest of them.
                        return; 
                    break;
                default:
                    cout << "WTF!!! See unit::learn()" << endl;
                    exit(-84); //let's leak some memory!!
                    break; //never hit.
            }
        }
    }
}
void unit::shit() //excrete is public. shit is private.
{
    if(map[y][x].waste<MAXWASTEONTILE)
        map[y][x].waste++;
    else
    {
        for(int i=0; ; i++) //searches for a square that isn't filled to the brim with excrement
        {
            bool good=false;
            for(int j=-i; j<=i; j++)
            {
                if(map[y+i][x+j].waste<MAXWASTEONTILE)
                {
                    map[y+i][x+j].waste++;
                    good=true;
                    break;
                }
                else if(map[y-i][x+j].waste<MAXWASTEONTILE)
                {
                    map[y-i][x+j].waste++;
                    good=true;
                    break;
                }
                else if(map[y+j][x+i].waste<MAXWASTEONTILE)
                {
                    map[y+j][x+j].waste++;
                    good=true;
                    break;
                }
                else if(map[y+j][x-i].waste<MAXWASTEONTILE)
                {
                    map[y+i][x-j].waste++;
                    good=true;
                    break;
                }
            }
            if(good)
                break;
        }
    }
    excreteNeed=-1;
    excreting=true;
}
//Lot of empty statements:D------------------------------------------------------------------------------------------------------------------------------------------------------------------------
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//public functions below --------------------------------------------------------------------------------------------------------------------------------------------------------------------------
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
unit& unit::operator=(const unit &source)
{
    player=source.player;
    index=source.index;
    sleep=source.sleep;
    energy=source.energy;
    strength=source.strength;
    gender=source.gender;
    intelligence=source.intelligence;
    age=source.age;
    moveToX=source.moveToX;
    x=source.x;
    moveToY=source.moveToY;
    y=source.y;   
    speed=source.speed;
    lineOfSight=source.lineOfSight;
    immunity=source.immunity;
    health=source.health;
    healthDiseaseInc=source.healthDiseaseInc;
    woundEnergyCost=source.woundEnergyCost;
    energyPerFood=source.energyPerFood;
    metabolicRate=source.metabolicRate;
    maxMetabolicRate=source.maxMetabolicRate;
    sexuallyMature=source.sexuallyMature;
    pregnant=source.pregnant;
    fetusid=source.fetusid;
    hunger=source.hunger;
    learningSkills=new short[NUMSKILLS]; //update every time there is a new skill
    for(int i=0; i<NUMSKILLS; i++)
        learningSkills[i]=source.learningSkills[i];
    
    sleeping=source.sleeping;
    reproducing=source.reproducing;
    moving=source.moving;  
    throwing=source.throwing;
    eating=source.eating;
    liftingOrDropping=source.liftingOrDropping;
    waking=source.waking;
    weight=source.weight;
    fatBuildProgress=source.fatBuildProgress;
    fatToWeight=source.fatToWeight;
    fatRetrievalEfficiency=source.fatRetrievalEfficiency;
    minWeight=NEWBORNMINWEIGHT;
    excreteNeed=-1;
    excreteNeedMax=source.excreteNeedMax;
    excreting=false;
    return *this;
}
void unit::move()
{
    if(sleeping || reproducing>0 || moving || throwing || waking || excreting)
        return;
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
    if(sleeping || reproducing>0 || moving || throwing || waking || excreting)
        return;
    if(mx==0 && my==0)
        return;
    if(index!=curLoops.unitIndex || player!=curLoops.unitPlayer)
        return;
    if(frames%speed!=0)
        return;
    if(abs(x-mx)<=1 && abs(y-my)<=1)
        moveHelper(mx-x,my-y);
}
void unit::reproduce(int withwhom)
{
    if(sleeping || reproducing>0 || moving || throwing || eating || liftingOrDropping || waking || excreting)
        return;
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(!allUnits.data[player][withwhom]) //mate is a null pointer
        return;
    unit* uwith=allUnits.get(this,withwhom,player);
    if(abs(x-uwith->x)<=1 && abs(y-uwith->y)<=1) //close enough
    {
        if(age>=sexuallyMature && uwith->age>uwith->sexuallyMature) //old enough
        {
            if(gender!=uwith->gender) //different genders
            {
                if(!gender) //female
                {
                    pregnant=0;
                    fetusid=allUnits.data[player].size();
                    unitChangeLog::update(x,y,player,index,0,0,0,-REPRODUCTIONENERGYCOST,0,0,1);
                    unitChangeLog::update(allUnits.data[player][withwhom]->x,allUnits.data[player][withwhom]->y,player,withwhom,0,0,0,-REPRODUCTIONENERGYCOST,0,0,0);
                }
                else //partner is female
                {
                    allUnits.data[player][withwhom]->pregnant=0;
                    allUnits.data[player][withwhom]->fetusid=allUnits.data[player].size();
                    unitChangeLog::update(x,y,player,index,0,0,0,-REPRODUCTIONENERGYCOST,0,0,0);
                    unitChangeLog::update(allUnits.data[player][withwhom]->x,allUnits.data[player][withwhom]->y,player,withwhom,0,0,0,-REPRODUCTIONENERGYCOST,0,0,1);
                }
                allUnits.data[player].push_back(new unit(player, allUnits.data.size(),geneMixer(strength,allUnits.data[player][withwhom]->strength),(bool)(rand()%2),geneMixer(intelligence,allUnits.data[player][withwhom]->intelligence),-1,-1,-1,(speed+allUnits.data[player][withwhom]->speed)/2,(lineOfSight+allUnits.data[player][withwhom]->lineOfSight)/2,geneMixer(immunity,allUnits.data[player][withwhom]->immunity),geneMixer(healthDiseaseInc,allUnits.data[player][withwhom]->healthDiseaseInc),geneMixer(woundEnergyCost,allUnits.data[player][withwhom]->woundEnergyCost),geneMixer(energyPerFood,allUnits.data[player][withwhom]->energyPerFood),geneMixer(metabolicRate,allUnits.data[player][withwhom]->metabolicRate),geneMixer(maxMetabolicRate,allUnits.data[player][withwhom]->maxMetabolicRate),(sexuallyMature+allUnits.data[player][withwhom]->sexuallyMature)/2,0,(rand()%4)+6,geneMixer(fatToWeight,allUnits.data[player][withwhom]->fatToWeight),geneMixer(fatRetrievalEfficiency,allUnits.data[player][withwhom]->fatRetrievalEfficiency),geneMixer(excreteNeedMax,allUnits.data[player][withwhom]->excreteNeedMax))); //adds the new unit. It doesn't really exist though
                if(allUnits.data[player][fetusid]->maxMetabolicRate>allUnits.data[player][fetusid]->metabolicRate)
                    allUnits.data[player][fetusid]->maxMetabolicRate=allUnits.data[player][fetusid]->metabolicRate-3;
                energy-=REPRODUCTIONENERGYCOST;
                allUnits.data[player][withwhom]->energy-=REPRODUCTIONENERGYCOST;
            }
        }
    }
    delete uwith;
}
void unit::goToSleep()
{
    if(sleeping || reproducing>0 || moving || throwing || eating || liftingOrDropping || waking || excreting)
        return;
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(map[y][x].waste>0 || map[y][x].bush>0) //bad ground for sleeping
        return;
    for(unsigned int i=0; i<map[y][x].allObjects.size(); i++)
    {
        if(!map[y][x].allObjects[i]->getsleepable(this))
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
void unit::pickUp(int what, int ox, int oy)
{
    if(sleeping || reproducing>0 || throwing || eating || liftingOrDropping || waking || excreting)
        return;
    if(abs(ox-x)>1 || abs(oy-y)>1) //too far
        return;
    if(index!=curLoops.unitIndex || player!=curLoops.unitPlayer)
        return;
    int carriedWeight=0;
    for(unsigned int i=0; i<carrying.size(); i++)
        carriedWeight+=carrying[i]->weight;
    for(unsigned int i=0; i<map[oy][ox].allObjects.size(); i++)
    {
        if(map[oy][ox].allObjects[i]->whatIsIt==what)
        {
            if(carriedWeight+map[oy][ox].allObjects[i]->weight<=strength*WEIGHTPERSTRENGTH) //good
            {
                carrying.push_back(map[oy][ox].allObjects[i]);
                carrying[carrying.size()-1]->x=-1;
                carrying[carrying.size()-1]->y=-1;
                carrying[carrying.size()-1]->heldByPlayer=player;
                carrying[carrying.size()-1]->heldByIndex=index;
                carrying[carrying.size()-1]->index=carrying.size()-1;
                map[oy][ox].allObjects.erase(map[oy][ox].allObjects.begin()+i);
            }
            break;
        }
    }
}
void unit::putDown(int objIndex, int px, int py)
{
    if(sleeping || reproducing>0 || moving || throwing || eating || liftingOrDropping || waking || excreting)
        return;
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(abs(x-px)>1 || abs(y-py)>1)
        return; //not allowed
    carrying[objIndex]->x=px;
    carrying[objIndex]->y=py;
    carrying[objIndex]->heldByIndex=-1;
    carrying[objIndex]->heldByPlayer=-1;
    carrying[objIndex]->index=map[y][x].allObjects.size();
    map[y][x].allObjects.push_back(carrying[objIndex]);
    carrying.erase(carrying.begin()+objIndex);
}
void unit::eat(int objIndex)
{
    if(sleeping || reproducing>0 || throwing || eating || liftingOrDropping || waking || excreting)
        return;
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(carrying[objIndex]->actuallyEdible==-3) // totally inedible
        return;
    for(unsigned int i=0; i<carrying[objIndex]->infected.size(); i++)
    {
        diseased.push_back(diseaseInfo(carrying[objIndex]->infected[i]));
        strength-=allDiseases[carrying[objIndex]->infected[i]].permStrCost;
        intelligence-=allDiseases[carrying[objIndex]->infected[i]].permIntelCost;
        immunity-=allDiseases[carrying[objIndex]->infected[i]].permImmunCost;
    }
    if(carrying[objIndex]->actuallyEdible>=0)
        hunger-=carrying[objIndex]->possFood.nutrition;
    if(hunger<0)
        hunger=0; //not entirely accurate, but there is some physical limit to how much you can eat. Here, there is no limit, but eventually it doesn't do anything when you eat.
}
void unit::throwObj(int objIndex, short atX, short atY, int strength)
{
    if(sleeping || reproducing>0 || throwing || eating || liftingOrDropping || waking || excreting)
        return;
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if((unsigned int)objIndex>=carrying.size())
        return;
    throwSkill.Throw(objIndex, this, atX, atY, moving, strength);
}
void unit::learnSkillFrom(int learnwhat, short fromwhom)
{
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    learningSkills[learnwhat]=fromwhom;
}
void unit::stopLearnSkillFrom(int learnwhat)
{
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    learningSkills[learnwhat]=-1;
}
void unit::excrete()
{
    if(sleeping || reproducing>0 || throwing || eating || liftingOrDropping || waking || excreting || moving)
        return;
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(excreteNeed>-1) //You are not allowed to produce waste whenever you want. Why? It seems like you wouldn't want to anyway, but this way you cannot go to someone else's city and wage biological warfare by constantly shiting on it.
        shit();
}
//getters 

vector<object> unit::getcarrying()
{
    vector<object> ret;
    if(curLoops.unitPlayer==player && curLoops.unitIndex==index) 
    { 
        for(unsigned int i=0; i<carrying.size(); i++)
            ret.push_back(*(carrying[i]));
        return ret;
    } 
    return ret; 
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
            
//getters for when seen by some other unit
#define Z(type, val) \
    type unit::getNonSelf ## val(unit* looking) \
    { \
        if(looking) \
        { \
            if(curLoops.unitIndex==looking->index && curLoops.unitPlayer==looking->player) \
            { \
                if(mapseenunit[looking->player][y][x].get(looking)==1) \
                    return val; \
            } \
        } \
        return (sizeof(type)==1)?(-127):(-9999); \
    } 
    LISTVARSUNITSEENBYOTHER
#undef Z                    