#include "creature.h"
#include "metabool.h"
#include "globals.h"
 
creature::creature()
{
#define W(type, var) \
    var = 0;
    LISTVARSCREATURE LISTVARSCREATURECONSTRUCTORONLY
#undef W
    index=-1;
	moveToX=x;
	moveToY=y;
	moving=false;
	sleeping=false;
	movingprog=0;
	fatBuildProgress=0;

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
	moving=false;
	sleeping=false;
	movingprog=0;

	currSeen = new vector<vector<metabool> >();
	currSeen->resize(lineOfSight*2+1);
	for(int i=0; i<lineOfSight*2+1; i++)
	{
		(*currSeen)[i].resize(lineOfSight*2+1);
	}
}

creature::~creature()
{
}

void creature::move()
{
    if(moving) //no moving twice in one frame.
        return; //did you think you won't get caught?
    if(moveToX==x && moveToY==y)
        return; //why did you want to do that. Lazy bum.
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
                    if((( ((map[y+curPoints[h].y][x+curPoints[h].x].bush>=125) ? 124 : map[y+curPoints[h].y][x+curPoints[h].x].bush) / 25 * CAMEOPER25BUSH) + (((map[y+curPoints[h].y][x+curPoints[h].x].tree>0)?1:0) * CAMEOFORTREE) + (((map[y+curPoints[h].y][x+curPoints[h].x].road>0)?1:0) * CAMEOFORROAD)) > (lineOfSight-((i>abs(j))?i:abs(j)))) //if the sum of the various cameo affects makes whatever unit is on that square invisible, make sure that happens
                        (*currSeen)[lineOfSight+curPoints[h].y][lineOfSight+curPoints[h].x].b=2;  //can see tile but not units on it
                    else
                        (*currSeen)[lineOfSight+curPoints[h].y][lineOfSight+curPoints[h].x].b=1;  //can see tile and units on it                  
                    ret = speciesIndex==0 ? map[y+curPoints[h].y][x+curPoints[h].x].blocksVision((unit*)this) : map[y+curPoints[h].y][x+curPoints[h].x].blocksVision((animal*)this); //if I saw an obstacle, add it to the list of obstacles
                    if(*ret)
                    {   
                    	obstacles.push_back(visionObstacle(curPoints[h].x,curPoints[h].y));
                    	delete ret; //delete the pointer. No memory leaks.
                    }
                }
            }
        }
    }
}
