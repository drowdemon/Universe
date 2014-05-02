#include <vector>
#include "animal.h"
#include "../tile.h"
#include "../globals.h"
#include "animalEatingAbilities.h"

animal::animal()
{
#define X(type, var) \
    var = 0;
    LISTVARSANIMAL
#undef X
    index=-1;
    searchDist=0;
	searchDir=0;
    moveToX=0;
    moveToY=0;
    moving=false;
    movingprog=0;
    eatingBehavior = NULL;
}


animal::animal(short p_health, char p_age, bool p_gender, short p_x, short p_y, short p_speed, short p_hunger, short p_index, short p_sleep, short p_weight, short p_lineOfSight, short p_animalType, animalEatingAbilities* p_eatingBehavior)
{
#define X(type, var) \
    var = p_ ## var ;     
    LISTVARSANIMAL
#undef X
    eatingBehavior=p_eatingBehavior;
}
bool animal::nextFrame()
{
    livingEvents();
    if(!checkLive())
    {
        die();
        return false;
    }
    see();
}
void animal::livingEvents()
{
    hunger--;
    sleep--;
    //make it autosleep on sleep=1
    animalChangeLog::update(x,y,index,0,0,0,1,1); 
}
bool animal::checkLive()
{
    if(hunger<0)
        return false;
    if(sleep<0)
        return false;
    if(health<0)
        return false;
    return true;
}
vector<vector<short> > *animal::searchFood()
{
    vector<vector<short> > *foodLocs = new vector<vector<short> >;
    foodLocs->resize(currSeen->size());
    for(unsigned int i=0; i<foodLocs->size(); i++)
    {
        (*foodLocs)[i].resize((*currSeen)[i].size());
        for(unsigned int j=0; j<(*foodLocs)[i].size(); j++)
            (*foodLocs)[i][j]=0;
    }
    if(foodapprox.size()==0)
    {
        if(searchDist==0)
        {
            searchDist=(rand()%(lineOfSight*3)) + lineOfSight;
            searchDir=rand()%8;
        }
        
        // <editor-fold defaultstate="collapsed" desc="NOT COMMENT: Changes in moveToVar based on searchDir">
        if(searchDir == 0)
            moveToX = x+1;
        else if(searchDir==1)
            moveToX=x-1;
        else if(searchDir==2)
            moveToY=y+1;
        else if(searchDir==3)
            moveToY=y-1;
        else if(searchDir==4)
        {
            moveToX=x+1;
            moveToY=y+1;
        }
        else if(searchDir==5)
        {
            moveToX=x-1;
            moveToY=y+1;
        }
        else if(searchDir==6)
        {
            moveToX=x+1;
            moveToY=y-1;
        }
        else if(searchDir==7)
        {
            moveToX=x-1;
            moveToY=y-1;
        }
        // </editor-fold>
        move(); //does not always move the animal
        if(x==moveToX && y==moveToY) //moved //Correct for case where there is an obstacle. Works otherwise because above movement is made quantized.
        {
            searchDist--;
            if(eatingBehavior->eatingBehavior(foodLocs)) //true = found food
            {
                foodapprox.push_back(point(x,y));
                return foodLocs;
            }
        }
    }
    else
    {
        searchDist=0;
        searchDir=0;
        if(x!=foodapprox.back().x || y!=foodapprox.back().y) //if not at a position in foodapprox
        {
            moveToX=foodapprox.back().x;
            moveToY=foodapprox.back().y;
            move();            
        }
        if(x==foodapprox.back().x && y==foodapprox.back().y) //already there
        {
            eatingBehavior->eatingBehavior(foodLocs);
        }
    }
    return foodLocs;
}
/*bool animal::searchFoodHelperBushes(vector<vector<short> > * foodLocs)
{
    bool ret=false;
    for(unsigned int i=0; i<currSeen->size(); i++)
    {
        for(unsigned int j=0; j<currSeen->size(); j++)
        {
            if((*currSeen)[i][j].b!=0) //can see this tile
            {
               (*foodLocs)[i][j]=map[y+i-lineOfSight][x+j-lineOfSight].bush-1; //has food
               if((*foodLocs)[i][j]>0)
                   ret=true;
            }
        }
    }
    return ret;
}*/
void animal::move()
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
void animal::moveHelper(int mx, int my)
{
    if(map[y+my][x+mx].walkable(this))
    {   
        int damage=0;
        if(map[y][x].height-map[y+my][x+mx].height>1) //possibly painful height differential
            health-=(damage=(int)((double)(map[y][x].height-map[y+my][x+mx].height-1)*(double)FALLINGMULTIPLIER));
        
        map[y][x].animalPresent=0;
        x+=mx;
        y+=my;
        
        map[y][x].animalPresent=index+1;
    }
    else
    {
        moveToX=x;
        moveToY=y;
    }
}
void animal::see()
{
    for(unsigned int i=0; i<currSeen->size(); i++)
    {
        for(unsigned int j=0; j<currSeen->size(); j++)
            (*currSeen)[i][j].b=0;
    }
    vector<visionObstacle> obstacles; //stores slope to obstacles
    (*currSeen)[y][x].b=1; //own square is completely visable
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
                        (*currSeen)[y+curPoints[h].y][x+curPoints[h].x].b=2;  //can see tile but not units on it
                    else
                        (*currSeen)[y+curPoints[h].y][x+curPoints[h].x].b=1;  //can see tile and units on it                  
                    ret = map[y+curPoints[h].y][x+curPoints[h].x].blocksVision(this); //if I saw an obstacle, add it to the list of obstacles
                    if(*ret)
                        obstacles.push_back(visionObstacle(curPoints[h].x,curPoints[h].y));
                    delete ret; //delete the pointer. No memory leaks.
                }
            }
        }
    }
}
void animal::die()
{
    map[y][x].animalPresent=0;
    map[y][x].allObjects.push_back(new object(allObjectDesc[OBJECT_GENERICANIMALCORPSE],-1,-1,x,y,map[y][x].allObjects.size(),map[y][x].height));
    map[y][x].allObjects.back()->weight=weight;
    delete allAnimals[index];
}

#define X(type, val) \
type animal::get ## val(animal *a) \
{ \
    if(a->index==curLoops.animalIndex) \
        return a->val; \
    return -127; \
}
LISTVARSANIMAL
#undef X
vector<vector<metabool> > *animal::getcurrSeen(animal* a)
{
    if(a->index==curLoops.animalIndex) 
    {
        vector<vector<metabool> > *ret = new vector<vector<metabool> >;
        (*ret) = *currSeen;
        return ret;
    }
    return NULL; 
}
vector<point> *animal::getfoodapprox(animal* a)
{
    if(a->index==curLoops.animalIndex) 
    {
        vector<point> *ret = new vector<point>;
        (*ret) = foodapprox;
        return ret;
    }
    return NULL; 
}