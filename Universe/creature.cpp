#include "creature.h"
#include "metabool.h"
#include "globals.h"
//#include "unit.h"
//#include "Animalia/animal.h"
 
creature::creature()
{
#define W(type, var) \
    var = 0;
    LISTVARSCREATURE LISTVARSCREATURECONSTRUCTORONLY
#undef W
    index=-1;
	moveToX=x;
	moveToY=y;
	fetusid=-1;
	fatBuildProgress=0;
	seeDir=0;
	
	sleeping=false;
	movingprog=0;
	reproducing=0;
	seeingIntently=0;
	moving=false;
	waking=false;
	liftingOrDropping=false;

	currSeen = new vector<vector<metabool> >();
	currSeen->resize(lineOfSight*2+1);
	for(int i=0; i<lineOfSight*2+1; i++)
	{
		(*currSeen)[i].resize(lineOfSight*2+1);
	}
}


#define W(type, val) \
type p_ ## val,    
creature::creature(LISTVARSCREATURE LISTVARSCREATURECONSTRUCTORONLY bool extraneous)
{
#undef W
#define W(type, var) \
    var = p_ ## var ;     
    LISTVARSCREATURE LISTVARSCREATURECONSTRUCTORONLY
#undef W
    
	moveToX=x;
	moveToY=y;
	fetusid=-1;
	fatBuildProgress=0;
	moving=false;
	waking=false;
	sleeping=false;
	liftingOrDropping=false;
	movingprog=0;
	reproducing=0;
	seeingIntently=0;

	currSeen = new vector<vector<metabool> >();
	currSeen->resize(lineOfSight*2+1);
	for(int i=0; i<lineOfSight*2+1; i++)
	{
		(*currSeen)[i].resize(lineOfSight*2+1);
	}
}

creature::~creature()
{
	for(unsigned int i=0; i<carrying.size(); i++)
    {
    	if(carrying[i])
        {
    		delete carrying[i];
    		carrying[i]=NULL;
        }
    }
    carrying.clear();
    delete currSeen;
}

void creature::move()
{
    if(moving) //no moving twice in one frame.
        return; //did you think you won't get caught?
    if(moveToX==x && moveToY==y)
        return; //why did you want to do that. Lazy bum.
    if(seeingIntently==1 && !sameDirSeeMove(seeDir, moveToX-x, moveToY-y)) //trying to look in a different direction, can't move. If looking in the same direction, can move 
    	return;    
    movingprog++;
    moving=true;
    if(movingprog==speed)
    {
        movingprog=0;
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

bool creature::checkLive(short maxHunger)
{
	if(health<0) 
		return false; //death by physical damage. 
	if(energy<0)
		return false; //death by energy loss. Unlikely to happen unless you are diseased. This is how diseases kill.
	if(sleep<0)
		return false; //death by extreme sleep deprivation
	if(hunger>=maxHunger) 
		return false; //death by starvation
	return true;
}

void creature::livingEvents(int speciesIndex)
{
    if(sleeping)
        sleep+=3;
    else
        sleep--;
    energy-=(allSpecies[speciesIndex].maxHealth-health)*woundEnergyCost;
    diseaseEffects(); //just energy costs
    if(energy<allSpecies[speciesIndex].energyFromFatPoint)
    {
        if(weight>minWeight && frames%allSpecies[speciesIndex].energyFromFatRate==0)
        {
            weight--;
            energy+=fatToWeight*fatRetrievalEfficiency/1000;
        }
    }
    if(energy<allSpecies[speciesIndex].energyCritPoint)
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
    {
        pregnant++;
        energy-=allSpecies[speciesIndex].pregnantEnergyCost*(2*(pregnant/1000));
    }
    if(reproducing>0)
    {
        reproducing++;
        if(pregnant>0)
            pregnant--; //not yet
        if(reproducing>allSpecies[speciesIndex].reproductionTime)
            reproducing=0; //done
    }
    if(pregnant>=allSpecies[speciesIndex].gestationPeriod) //birth
    {
        giveBirth(); //changes health and pregnant
    }
    if(energy>allSpecies[speciesIndex].energySoftMax)
    {
        energy--;
        fatBuildProgress++;
        if(fatBuildProgress>=fatToWeight)
        {
            fatBuildProgress=0;
            weight++; 
        }
    }
    /*if(excreteNeed>=0)
    {
        excreteNeed++;
        if(excreteNeed>=excreteNeedMax)
            shit();
    }*/
    //unitChangeLog::update(x,y,player,index,0,0,health-deltaHlth,energy-deltaE,hunger-deltaH,sleep-deltaS,pregnant-deltaP,NULL);
}

void creature::see()
{
    for(unsigned int i=0; i<currSeen->size(); i++)
    {
        for(unsigned int j=0; j<currSeen->size(); j++)
            (*currSeen)[i][j].b=0;
    }
    vector<visionObstacle> obstacles; //stores slope to obstacles
    (*currSeen)[lineOfSight][lineOfSight].b=1; //own square is completely visable
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
                        if(obstacles[k].slope1>obstacles[k].slope2) //Q I or Q II   //Also includes all times when obstacle is on an axis
                        {
                            if((curPoints[h].y<0) || (obstacles[k].y==0 || obstacles[k].x==0)) //correct quadrant, or on axis. y flipped because everything is flipped
                            {
                                if((obstacles[k].slope2<curSlopes[h] && curSlopes[h]<obstacles[k].slope1 && obstacles[k].x!=0) || ((obstacles[k].slope2>curSlopes[h] || curSlopes[h]>obstacles[k].slope1) && obstacles[k].x==0)) //between two slopes of obstacle: unseen
                                {
                                    if((obstacles[k].y==0 && ((curPoints[h].x<0)==(obstacles[k].x<0))) || (obstacles[k].x==0 && ((curPoints[h].y<0)==(obstacles[k].y<0))) || (obstacles[k].x!=0 && obstacles[k].y!=0)) //extra test for the points on an axis //check this
                                        allowed[h]=false;
                                }
                            }
                        }
                        else //Q III,Q IV //s2<s1.
                        {
                            if(curPoints[h].y>0) //y flipped b/c everything is flipped. y axis goes negative->positive top->down
                            {
                                if(obstacles[k].slope1<curSlopes[h] && curSlopes[h]<obstacles[k].slope2)
                                    allowed[h]=false;
                            }
                        }
                    }
                    else
                    {
                        if(curPoints[h].x==obstacles[k].x && abs(curPoints[h].y)>abs(obstacles[k].y) && ((curPoints[h].y<0)==(obstacles[h].y<0))) //point is greater than obstacle and has the same sign as the obstacle
                            allowed[h]=false;
                    }
                }
            }
            bool *ret;
            for(int h=0; h<4; h++) //for all 4 points I'm checking
            {
                if(allowed[h]) //if its not behind an obstacle
                {
                	int loPs = allSpecies[speciesIndex].lineOfPerfectSight;
                	if(seeingIntently==1 && sameDirSeeMove(seeDir, curPoints[h].x, curPoints[h].y)) //makes seeIntently do something
                		loPs=lineOfSight; //effectively makes the second condition never hit. Which means that if it's visible, you can see it, none of this probability nonsense, which is the point
                	
                	int distance = (i>abs(j) ? i : abs(j)) - map[y+curPoints[h].y][x+curPoints[h].x].cameouflageAmnt();
                    if(distance > lineOfSight) //if the sum of the various cameo affects makes whatever unit is on that square invisible, make sure that happens
                        (*currSeen)[lineOfSight+curPoints[h].y][lineOfSight+curPoints[h].x].b=2;  //can see tile but not units on it
                    else if(distance > loPs) //It might be possible to see it, but it might just be interpretted as 'something is there, who the hell knows what'
                    {
                    	int prob = (lineOfSight-distance)*(lineOfSight-distance)*coefOfWorseningSight;
                    	if((rand()%10000) < prob)
                    		(*currSeen)[lineOfSight+curPoints[h].y][lineOfSight+curPoints[h].x].b=1;  //can see tile and units on it
                    	else
                    		(*currSeen)[lineOfSight+curPoints[h].y][lineOfSight+curPoints[h].x].b=3;  //can see the tile, and know that there is *something* on it. Acts like it being 2, but gives you a little more info.
                    }
                    else
                        (*currSeen)[lineOfSight+curPoints[h].y][lineOfSight+curPoints[h].x].b=1;  //can see tile and units on it                  
                    ret = speciesIndex==0 ? map[y+curPoints[h].y][x+curPoints[h].x].blocksVision((unit*)this) : map[y+curPoints[h].y][x+curPoints[h].x].blocksVision((animal*)this); //if I saw an obstacle, add it to the list of obstacles
                    if(ret && *ret)
                    {   
                    	obstacles.push_back(visionObstacle(curPoints[h].x,curPoints[h].y));
                    }
                    if(ret)
                    	delete ret; //delete the pointer. No memory leaks.
                }
            }
        }
    }
}
vector<point> creature::seeGUI()
{
    vector<point> toreturn;
    see();
    for(unsigned int i=0; i<currSeen->size(); i++)
    {
    	for(unsigned int j=0; j<currSeen->size(); j++)
    	{
    		if(speciesIndex==0)
    		{
    			if((*currSeen)[i][j].get((unit*)this)>0)
    				toreturn.push_back(point(x+j-lineOfSight, y+i-lineOfSight));
    		}
    		else
    		{
    			if((*currSeen)[i][j].get((animal*)this)>0)
					toreturn.push_back(point(x+j-lineOfSight, y+i-lineOfSight));
    		}
    	}
    }
    return toreturn;
}
void creature::resetActions()
{
	waking=false;
	moving=false;
	if(seeingIntently>0)
		seeingIntently--;
}
void creature::awaken()
{
	if(!sleeping) //can't possibly have been doing anything else anyway, no need to check for multitasking.
	    return;
    if((speciesIndex==0 && (((unit*)this)->player!=curLoops.unitPlayer || index!=curLoops.unitIndex)) || (speciesIndex!=0 && index!=curLoops.animalIndex))
        return;
    sleeping=false;
    waking=true;
}
void creature::reproduce(int withwhom, creature *cwith)
{
	if(abs(x-cwith->x)<=1 && abs(y-cwith->y)<=1) //close enough
    {
        if(age>=sexuallyMature && cwith->age>cwith->sexuallyMature) //old enough
        {
            if(gender!=cwith->gender) //different genders
            {
                if(!gender) //female
                {
                    pregnant=0;
                    short player=-1;
                    if(speciesIndex==0)
                    {
                    	player=((unit*)(this))->player;
                    	fetusid=allUnits.data[player].size();
                    }
                    else
                    {
                    	fetusid=allAnimals.size();
                    }
                    creatureChangeLog::update(x,y,player,index,0,0,0,-REPRODUCTIONENERGYCOST,0,0,1,NULL);
                    creatureChangeLog::update(cwith->x,cwith->y,player,withwhom,0,0,0,-REPRODUCTIONENERGYCOST,0,0,0,NULL);
                }
                else //partner is female
                {
                    cwith->pregnant=0;
                    short player=-1;
                    if(speciesIndex==0)
                    {
                    	player=((unit*)(cwith))->player;
                    	fetusid=allUnits.data[player].size();
                    }
                    else
                    {
                    	fetusid=allAnimals.size();
                    }
                    creatureChangeLog::update(x,y,player,index,0,0,0,-REPRODUCTIONENERGYCOST,0,0,0,NULL);
                    creatureChangeLog::update(cwith->x,cwith->y,player,withwhom,0,0,0,-REPRODUCTIONENERGYCOST,0,0,1,NULL);
                }
                creature* fetus = createFetus(withwhom);
                if(fetus->maxMetabolicRate > fetus->metabolicRate) //max metabolic rate must be less than metabolic rate. Otherwise it's stupid
                {
                	fetus->maxMetabolicRate = fetus->metabolicRate-3;
                	if(fetus->maxMetabolicRate < 1) //and it can't be < 1
                		fetus->maxMetabolicRate=1;
                }
                energy-=REPRODUCTIONENERGYCOST; //if you die, you die. Your problem.
                cwith->energy-=REPRODUCTIONENERGYCOST;
            }
        }
    }
}
bool creature::sameDirSeeMove(short dirSee, short dx, short dy)
{
	if((dirSee==1 && dy<0) || (dirSee==2 && dy>0)) //up/down
		return true;
	if((dirSee==3 && dx<0) || (dirSee==4 && dx>0)) //left/right
		return true;
	return false;
}
void creature::seeIntently(short dirSee)
{
	if(dirSee<=0 || 4<dirSee) //invalid direction
		return;
	if(moving && !sameDirSeeMove(seeDir, moveToX-x, moveToY-y))  //trying to move in a different direction, can't look. If moving in the same direction, can look
		return;
	seeingIntently=2;
	seeDir=dirSee;
}
void creature::infect()
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
                        if(speciesIndex==0)
                        {
                        	((unit*)this)->intelligence-=allDiseases[i].permIntelCost;
                        }
                        immunity-=allDiseases[i].permImmunCost;
                        strength-=allDiseases[i].permStrCost;
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
                        if(map[i][j].uniton && ((speciesIndex==0 && (allDiseases[diseased[h]].transmit&CONTACT_TRANSMIT)>0) || (speciesIndex!=0 && (allDiseases[diseased[h]].transmit&ANIMAL_TRANSMIT)>0))) //theres a unit and ((transmitable unit-unit and I'm a unit) or (transmitable unit-animal and I'm an animal))
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
                        else if(map[i][j].animalPresent>0 && ((speciesIndex==0 && (allDiseases[diseased[h]].transmit&ANIMAL_TRANSMIT)>0) || (speciesIndex!=0 && (allDiseases[diseased[h]].transmit&CONTACT_TRANSMIT)>0)))
                        {
                            if(map[i][j].x!=x || map[i][j].y!=y) //different unit
                            {
                                int tempimmunloss=0;
                                for(unsigned int d=0; d<allAnimals[map[i][j].animalPresent-1]->diseased.size(); d++)
                                    tempimmunloss+=allDiseases[allAnimals[map[i][j].animalPresent-1]->diseased[d]].immunCost;
                                if(rand()%10000<allDiseases[diseased[h]].spreadabilityChance-((allAnimals[map[i][j].animalPresent-1]->immunity-tempimmunloss>0)?(allAnimals[map[i][j].animalPresent-1]->immunity-tempimmunloss):0)+((MAXHEALTH-allAnimals[map[i][j].animalPresent-1]->health)*allAnimals[map[i][j].animalPresent-1]->healthDiseaseInc))
                                {
                                    bool good=true;
                                    for(unsigned int d=0; d<allAnimals[map[i][j].animalPresent-1]->diseased.size(); d++)
                                    {
                                        if(diseased[h]==allAnimals[map[i][j].animalPresent-1]->diseased[d]) //you actually managed to get sick with the same disease TWICE! Good for you!
                                        {
                                            good=false;
                                            allAnimals[map[i][j].animalPresent-1]->diseased[d].multiplier++; //makes the disease more fearsome. Or at least more advanced in development
                                            allAnimals[map[i][j].animalPresent-1]->diseased[d].flipDir=false; //If you were getting better, screw that! Now you're getting sicker again!
                                            break;
                                        }
                                    }
                                    if(good)
                                    {
                                        allAnimals[map[i][j].animalPresent-1]->diseased.push_back(diseased[h].disease);
                                        allAnimals[map[i][j].animalPresent-1]->immunity-=allDiseases[diseased[h]].permImmunCost;
                                        allAnimals[map[i][j].animalPresent-1]->strength-=allDiseases[diseased[h]].permStrCost;
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
void creature::diseaseEffects()
{
    int start=energy;
    for(unsigned int i=0; i<diseased.size(); i++)
        energy-=allDiseases[diseased[i]].energyCost;
    if(energy-start!=0)
        creatureChangeLog::update(x,y, speciesIndex==0 ? ((unit*)this)->player : -1 ,index,0,0,0,(energy-start),0,0,0,NULL);
}
void creature::hitWithFlyingObject(int objIndex) //add more factors to the damage. Object sharpness maybe. How hard/soft it is. 
{
    health-=map[y][x].allObjects[objIndex]->speed*SPEEDTODAMAGE; 
}
void creature::pickUp(int what, int ox, int oy)
{
	if(abs(ox-x)>1 || abs(oy-y)>1) //too far
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
                
                carrying[carrying.size()-1]->heldByPlayer = speciesIndex==0 ? ((unit*)this)->player : -1;
                
                carrying[carrying.size()-1]->heldByIndex=index;
                carrying[carrying.size()-1]->index=carrying.size()-1;
                map[oy][ox].allObjects.erase(map[oy][ox].allObjects.begin()+i);
            }
            break;
        }
    }
    liftingOrDropping=true;
}
void creature::putDown(int objIndex, int px, int py)
{
	if(abs(x-px)>1 || abs(y-py)>1)
        return; //not allowed
    carrying[objIndex]->x=px;
    carrying[objIndex]->y=py;
    carrying[objIndex]->heldByIndex=-1;
    carrying[objIndex]->heldByPlayer=-1;
    carrying[objIndex]->index=map[y][x].allObjects.size();
    map[y][x].allObjects.push_back(carrying[objIndex]);
    carrying.erase(carrying.begin()+objIndex);
    liftingOrDropping=true;
}
