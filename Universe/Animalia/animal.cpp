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
    currSeen=NULL;
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
#define X(type, var) var = p_ ## var ;     
    LISTVARSANIMAL
#undef X
    eatingBehavior=p_eatingBehavior;
    
	searchDist=0;
	searchDir=0;
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
    resetActions();
    return true;
}
void animal::livingEvents(int p_speciesIndex)
{
	int deltaE=energy;
	int deltaH=hunger;
	int deltaP=pregnant;
	int deltaS=sleep;
	int deltaHlth=health;
	creature::livingEvents(speciesIndex);
	
	creatureChangeLog::update(x,y,-1,index,0,0,health-deltaHlth,energy-deltaE,hunger-deltaH,sleep-deltaS,pregnant-deltaP,NULL);
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
        
        energy-=allSpecies[speciesIndex].movementEnergy;
        energy-=weight/50*allSpecies[speciesIndex].movingSelfWeightPenalty;
        energy-=((rand()%50)<(weight%50)) ? allSpecies[speciesIndex].movingSelfWeightPenalty : 0;
        
        map[y][x].animalPresent=index+1;
        
        vector<point> seensq = seeGUI();
        
        creatureChangeLog::update(x-mx,y-my,-1,index,mx,my,-damage,MOVEMENTENERGY,0,0,0,&seensq);
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
    
    int i = index;
    
    delete allAnimals[index];
    allAnimals[i] = NULL;
    vector<point> *t= new vector<point>();
    creatureChangeLog::update(-99999,-99999,-1,i,-99999,-99999,-99999,-99999,-99999,-99999,-99999,t);
    delete t;
}
void animal::giveBirth()
{
    pregnant=-1;
    health-=BIRTHHEALTHLOSS;
    energy-=BIRTHENERGYLOSS;
    animal* child=allAnimals[fetusid];
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
    map[child->y][child->x].animalPresent=child->index+1;
    creatureChangeLog::update(child->x,child->y,-1,child->index,0,0,child->health,child->energy,child->hunger,child->sleep,child->pregnant,NULL);
}
void animal::act()
{
	
}
void animal::reproduce(int withwhom)
{
    if(sleeping || reproducing>0 || moving || waking)
        return;
    if(index!=curLoops.animalIndex)
        return;
    if(!allAnimals[withwhom]) //mate is a null pointer
        return;
    creature* awith = allAnimals[withwhom];
    creature::reproduce(withwhom, awith);
}
void animal::goToSleep()
{
	if(sleeping || reproducing>0 || moving || waking)
		return;
	if(index!=curLoops.animalIndex)
		return;
	if(map[y][x].waste>0) //bad ground for sleeping
		return;
	sleeping=true;
}
creature* animal::createFetus(int withwhom)
{
	allAnimals.push_back(new animal((bool)(rand()%2), geneMixer(speed, allAnimals[withwhom]->speed), allAnimals.size(), geneMixer(lineOfSight, allAnimals[withwhom]->lineOfSight), allSpecies[speciesIndex].maxHealth, (rand()%4)+allSpecies[speciesIndex].newbornMinWeight, allSpecies[speciesIndex].newbornHunger, -1, -1, allSpecies[speciesIndex].newbornSleep, 0, allSpecies[speciesIndex].newbornEnergy, -1, speciesIndex, geneMixer(woundEnergyCost, allAnimals[withwhom]->woundEnergyCost), allSpecies[speciesIndex].newbornMinWeight, geneMixer(fatToWeight, allAnimals[withwhom]->fatToWeight), geneMixer(fatRetrievalEfficiency, allAnimals[withwhom]->fatRetrievalEfficiency), geneMixer(maxMetabolicRate, allAnimals[withwhom]->maxMetabolicRate), geneMixer(energyPerFood, allAnimals[withwhom]->energyPerFood), geneMixer(metabolicRate, allAnimals[withwhom]->metabolicRate), geneMixer(coefOfWorseningSight, allAnimals[withwhom]->coefOfWorseningSight),(sexuallyMature+allAnimals[withwhom]->sexuallyMature)/2, animalType, geneMixer(skittish, allAnimals[withwhom]->skittish), allSpecies[speciesIndex].eatingBehavior)); //adds a new animal
    return allAnimals[allAnimals.size()-1];
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
