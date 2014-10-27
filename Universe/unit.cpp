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

unit::unit() : creature(), throwSkill(0)
{
}
#define W(type, val) type p_ ## val,
#define U(type, val) W(type, val)
#define V(type, val) W(type, val)
unit::unit(LISTVARSCREATURE LISTVARSCREATURECONSTRUCTORONLY LISTVARSUNITCONSTRUCTOR LISTVARSUNITSKILLSCONSTRUCTOR bool extraneous) : creature(
		#undef W
		#define W(type, val) p_ ## val,
		LISTVARSCREATURE 
		LISTVARSCREATURECONSTRUCTORONLY
		false 
		), throwSkill(p_throwSkill)
{
#undef W
#undef U
#undef V
#define U(type, val) val = p_ ## val;
	LISTVARSUNITCONSTRUCTOR
#undef U 
    learningSkills=new short[NUMSKILLS]; //update every time there is a new skill
    for(int i=0; i<NUMSKILLS; i++)
        learningSkills[i]=-1;
    
    throwing=false;
    excreteNeed=-1;
    excreting=false;
}
unit::unit(const unit &source) : creature(source), throwSkill(source.throwSkill)
{
    player=source.player;
    intelligence=source.intelligence;
    excreteNeedMax=source.excreteNeedMax;
    
    learningSkills = new short[NUMSKILLS]; //update every time there is a new skill
    for(int i=0; i<NUMSKILLS; i++)
        learningSkills[i]=source.learningSkills[i];
    
    excreteNeed=source.excreteNeed;
    
    throwing=source.throwing;
    excreting=source.excreting;
}

unit::~unit()
{
	delete[] learningSkills;
}
bool unit::nextFrame()
{
    if(age==-1) //in womb
        return true; //complete successfully
    resetSkills();
    livingEvents(0);
    if(!checkLive(MAXHUNGER))
        return false;
    emergencySleep();
    for(unsigned int i=0; i<allMinds.data[player].size(); i++)
        seehive(i);
    see();
    if(!checkLive(MAXHUNGER))
    {
        for(unsigned int i=0; i<allMinds.data[player].size(); i++)
                unseehive(i);
        return false;
    }
    infect();
    int tempmovingprog=movingprog;
    act(); //AI
    if(movingprog==tempmovingprog) //did not move
        movingprog=0; //no progress on moving
    if(!checkLive(MAXHUNGER))
    { 
        for(unsigned int i=0; i<allMinds.data[player].size(); i++)
                unseehive(i);
        return false;
    }
    learn(); //learn if you set that you want to and you're near whoever you are learning from and they did something
    resetActions();
    return true;
}
void unit::moveHelper(int mx, int my)
{
    if(map[y+my][x+mx].walkable(this))
    {   
        int damage=0;
        if(map[y][x].height-map[y+my][x+mx].height > allSpecies[speciesIndex].tolerableHeightDiff) //possibly painful height differential
            health-=(damage=(int)((double)(map[y][x].height-map[y+my][x+mx].height-allSpecies[speciesIndex].tolerableHeightDiff)*(double)FALLINGMULTIPLIER));
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
        vector<point> seensq = seeGUI();
        for(unsigned int i=0; i<allMinds.data[player].size(); i++)
            seehive(i);
        creatureChangeLog::update(x-mx,y-my,player,index,mx,my,-damage,MOVEMENTENERGY,0,0,0,&seensq);
    }
    else
    {
        moveToX=x;
        moveToY=y;
    }
}
void unit::livingEvents(int speciesIndex)
{
    int deltaE=energy;
    int deltaH=hunger;
    int deltaP=pregnant;
    int deltaS=sleep;
    int deltaHlth=health;
    creature::livingEvents(0);
    if(hunger!=deltaH)
    {
    	if(hunger%EXCRETIONFREQ==0 && hunger!=0)
    		excreteNeed=0;
    }
    if(excreteNeed>=0)
    {
        excreteNeed++;
        if(excreteNeed>=excreteNeedMax)
            shit();
    }
    creatureChangeLog::update(x,y,player,index,0,0,health-deltaHlth,energy-deltaE,hunger-deltaH,sleep-deltaS,pregnant-deltaP,NULL);
}
void unit::seehive(int hiveindex)
{
    if(abs(allMinds.data[player][hiveindex].centerx-x)<allMinds.data[player][hiveindex].range && abs(allMinds.data[player][hiveindex].centery-y)<allMinds.data[player][hiveindex].range) //in range of hive
    {
        vector<visionObstacle> obstacles; //stores slope to obstacles
        mapseenhive[player][hiveindex][y][x].b=1; //own square is completely visable
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
                    	int distance = (i>abs(j) ? i : abs(j)) - map[y+curPoints[h].y][x+curPoints[h].x].cameouflageAmnt();
                        if(distance > lineOfSight) //if the sum of the various cameo affects makes whatever unit is on that square invisible, make sure that happens
                            mapseenhive[player][hiveindex][y+curPoints[h].y][x+curPoints[h].x].b=2;  //can see tile but not units on it
                        else if(distance > allSpecies[speciesIndex].lineOfPerfectSight)
                        {
                        	int prob = (lineOfSight-distance)*(lineOfSight-distance)*coefOfWorseningSight;
                        	if((rand()%10000) < prob)
                        		mapseenhive[player][hiveindex][y+curPoints[h].y][x+curPoints[h].x].b=1;  //can see tile and units on it     
                        	else
                        		mapseenhive[player][hiveindex][y+curPoints[h].y][x+curPoints[h].x].b=3;  //can see the tile, and know that there is *something* on it. Acts like it being 2, but gives you a little more info.     
                        }
                        else
                            mapseenhive[player][hiveindex][y+curPoints[h].y][x+curPoints[h].x].b=1;  //can see tile and units on it
                        ret = map[y+curPoints[h].y][x+curPoints[h].x].blocksVision(this);
                        if(ret && *ret) //if I saw an obstacle, add it to the list of obstacles
                            obstacles.push_back(visionObstacle(curPoints[h].x,curPoints[h].y));
                        if(ret)
                        	delete ret; //delete the pointer. No memory leaks.
                    }
                }
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
    move(); //remove this line. TESTING ONLY!!!
    /*if(index==0 && frames==1)
        throwObj(0,x+1,y+1,strength);*/
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
    creatureChangeLog::update(child->x,child->y,child->player,child->index,0,0,child->health,child->energy,child->hunger,child->sleep,child->pregnant,NULL);
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
        carrying[i]=NULL; //the pointer belongs to the map now. This way it won't be deleted when we delete the unit.
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
    vector<point> *t= new vector<point>();
    creatureChangeLog::update(-99999,-99999,p,i,-99999,-99999,-99999,-99999,-99999,-99999,-99999,t);
    delete t;
}
void unit::resetActions() //sleeping is controlled differently, and reproduction is in livingEvents()
{
	creature::resetActions();
    throwing=false;
    eating=false;
    liftingOrDropping=false;
    excreting=false;
}
void unit::resetSkills()
{
    throwSkill.threw=false;
}
void unit::learn()
{
    if(sleeping || reproducing>0 || throwing || liftingOrDropping || waking || excreting || seeingIntently==1) //eating and moving are ok
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
creature* unit::createFetus(int withwhom)
{
	allUnits.data[player].push_back(new unit((bool)(rand()%2), geneMixer(speed, allUnits.data[player][withwhom]->speed), geneMixer(strength, allUnits.data[player][withwhom]->strength), allUnits.data[player].size(), geneMixer(lineOfSight, allUnits.data[player][withwhom]->lineOfSight), allSpecies[0].maxHealth, (rand()%4)+allSpecies[0].newbornMinWeight, allSpecies[0].newbornHunger, -1, -1, allSpecies[0].newbornSleep, 0, allSpecies[0].newbornEnergy, -1, 0, geneMixer(woundEnergyCost, allUnits.data[player][withwhom]->woundEnergyCost), allSpecies[0].newbornMinWeight, geneMixer(fatToWeight, allUnits.data[player][withwhom]->fatToWeight), geneMixer(fatRetrievalEfficiency, allUnits.data[player][withwhom]->fatRetrievalEfficiency), geneMixer(maxMetabolicRate, allUnits.data[player][withwhom]->maxMetabolicRate), geneMixer(energyPerFood, allUnits.data[player][withwhom]->energyPerFood), geneMixer(metabolicRate, allUnits.data[player][withwhom]->metabolicRate), geneMixer(coefOfWorseningSight, allUnits.data[player][withwhom]->coefOfWorseningSight),(sexuallyMature+allUnits.data[player][withwhom]->sexuallyMature)/2, geneMixer(immunity,allUnits.data[player][withwhom]->immunity), geneMixer(healthDiseaseInc, allUnits.data[player][withwhom]->healthDiseaseInc), player, geneMixer(intelligence,allUnits.data[player][withwhom]->intelligence), geneMixer(excreteNeedMax, allUnits.data[player][withwhom]->excreteNeedMax), 0)); //adds the new unit. It doesn't really exist though
    return allUnits.data[player][allUnits.data[player].size()-1]; 
}
//Lot of empty statements:D------------------------------------------------------------------------------------------------------------------------------------------------------------------------
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//public functions below --------------------------------------------------------------------------------------------------------------------------------------------------------------------------
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void unit::move() //make moving an int to force continual
{
    if(index!=curLoops.unitIndex || player!=curLoops.unitPlayer)
        return;
    if(sleeping || reproducing>0 || moving || waking || excreting || liftingOrDropping) //eating and throwing are allowed (though not both at once). SeeingIntently taken care of in creature::move 
        return;
    creature::move();
}
void unit::move(short mx, short my)
{
    if(sleeping || reproducing>0 || moving  || waking || excreting || liftingOrDropping) //eating and throwing are allowed (though not both at once)
        return;
    if(mx==0 && my==0)
        return;
    if(index!=curLoops.unitIndex || player!=curLoops.unitPlayer)
        return;
    movingprog++;
    moving=true;
    if(movingprog!=speed)
        return;
    movingprog=0;
    if(abs(x-mx)<=1 && abs(y-my)<=1)
        moveHelper(mx-x,my-y);
}
void unit::reproduce(int withwhom)
{
    if(sleeping || reproducing>0 || moving || throwing || eating || liftingOrDropping || waking || excreting || seeingIntently==1) //nothing else allowed. Serious business.
        return;
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(!allUnits.data[player][withwhom]) //mate is a null pointer
        return;
    creature* uwith = allUnits.data[player][withwhom];
    creature::reproduce(withwhom, uwith);
}
void unit::goToSleep()
{
    if(sleeping || reproducing>0 || moving || throwing || eating || liftingOrDropping || waking || excreting || seeingIntently==1) //nothing else allowed
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
void unit::awaken() //makes it public
{
	creature::awaken();
}
void unit::pickUp(int what, int ox, int oy)
{
    if(sleeping || reproducing>0 || throwing || eating || liftingOrDropping || waking || excreting || moving || seeingIntently==1) //nothing else is allowed
        return;
    if(index!=curLoops.unitIndex || player!=curLoops.unitPlayer)
        return;
    creature::pickUp(what, ox, oy);
}
void unit::putDown(int objIndex, int px, int py)
{
    if(sleeping || reproducing>0 || moving || throwing || eating || liftingOrDropping || waking || excreting || seeingIntently==1) //nothing else is allowed
        return;
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    creature::putDown(objIndex, px, py);
}
void unit::eat(int objIndex)
{
    if(sleeping || reproducing>0 || throwing || eating || liftingOrDropping || waking || excreting || seeingIntently==1) //moving is allowed
        return;
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(carrying[objIndex]->actuallyEdible==-3) // totally inedible
        return;
    for(unsigned int i=0; i<carrying[objIndex]->infected.size(); i++)
    {
    	//TODO maybe make this happen with spreadabilityChance probability?
        diseased.push_back(diseaseInfo(carrying[objIndex]->infected[i]));
        strength-=allDiseases[carrying[objIndex]->infected[i]].permStrCost;
        intelligence-=allDiseases[carrying[objIndex]->infected[i]].permIntelCost;
        immunity-=allDiseases[carrying[objIndex]->infected[i]].permImmunCost;
    }
    if(carrying[objIndex]->actuallyEdible>=0)
        hunger-=carrying[objIndex]->possFood->nutrition;
    if(hunger<0)
        hunger=0; //not entirely accurate, but there is some physical limit to how much you can eat. Here, there is no limit, but eventually it doesn't do anything when you eat.
    //ate the object, it's gone now.
    delete carrying[objIndex];
    carrying[objIndex]=NULL;
    carrying.erase(carrying.begin()+objIndex);
}
void unit::throwObj(int objIndex, short atX, short atY, int strength)
{
    if(sleeping || reproducing>0 || throwing || eating || liftingOrDropping || waking || excreting || seeingIntently==1) //moving is allowed
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
    if(learnwhat>=NUMSKILLS) //fuck off
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
    if(sleeping || reproducing>0 || throwing || eating || liftingOrDropping || waking || excreting || moving || seeingIntently==1) //nothing else is allowed
        return;
    if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
        return;
    if(excreteNeed>-1) //You are not allowed to produce waste whenever you want. Why? It seems like you wouldn't want to anyway, but this way you cannot go to someone else's city and wage biological warfare by constantly shiting on it.
        shit();
}
void unit::seeIntently(short dirSee)
{
	if(sleeping || reproducing>0 || throwing || eating || liftingOrDropping || waking || excreting || seeingIntently==1) //moving is taken care of in creature, besides that nothing else is allowed 
		return;
	if(player!=curLoops.unitPlayer || index!=curLoops.unitIndex)
		return;
	creature::seeIntently(dirSee);
}

//getters 
vector<object*> unit::getcarrying()
{
    vector<object*> ret;
    if(curLoops.unitPlayer==player && curLoops.unitIndex==index) 
    { 
        for(unsigned int i=0; i<carrying.size(); i++)
            ret.push_back(new object(*(carrying[i])));
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
            if(curLoops.unitIndex==looking->index && curLoops.unitPlayer==looking->player && abs(looking->x-x) > looking->lineOfSight && abs(looking->y-y) > looking->lineOfSight) \
            { \
                if((*currSeen)[looking->lineOfSight+looking->y-y][looking->lineOfSight+looking->x-x].get(looking)==1) \
                    return val; \
            } \
        } \
        return (sizeof(type)==1)?(-127):(-9999); \
    } 
    LISTVARSUNITSEENBYOTHER
#undef Z                    
