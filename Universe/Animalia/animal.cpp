#include <vector>
#include "animal.h"
#include "../tile.h"
#include "../globals.h"
#include "animalEatingAbilities.h"

animal::animal() : creature()
{
#define X(type, var) var = 0;
    LISTVARSANIMAL
#undef X
    searchDist=0;
	searchDir=0;
    eatingBehavior = NULL;
}

#define X(type, val)				\
    type p_ ## val,    
#define W(type, val) X(type, val)
animal::animal(LISTVARSCREATURE LISTVARSCREATURECONSTRUCTORONLY LISTVARSANIMAL animalEatingAbilities *p_eatingBehavior) : creature(

		#undef W
		#define W(type, val) p_ ## val,
		LISTVARSCREATURE 
		LISTVARSCREATURECONSTRUCTORONLY
		false )
		#undef W
{
#undef X
#define X(type, var) \
    var = p_ ## var ;     
    LISTVARSANIMAL
#undef X
    eatingBehavior=p_eatingBehavior;
    
    index=-1;
	searchDist=0;
	searchDir=0;
	moveToX=0;
	moveToY=0;
	moving=false;
	movingprog=0;

	currSeen = new vector<vector<metabool> >();
	currSeen->resize(lineOfSight*2+1);
	for(int i=0; i<lineOfSight*2+1; i++)
	{
		currSeen[i].resize(lineOfSight*2+1);
	}
}
bool animal::nextFrame()
{
    livingEvents(speciesIndex);
    if(!checkLive(allSpecies[speciesIndex].maxHunger))
    {
        die();
        return false;
    }
    see();
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
void animal::giveBirth()
{
	
}
void animal::act()
{
	
}
