#include "tile.h"
#include "unit.h"
#include "globals.h"
#include "metabool.h"
#include <cmath>
#include <cstdlib>
#include "hivemind.h"
#include "allunits.h"
#include "dataStructures.h"
#include "Animalia/animal.h"

tile::tile(unsigned char r, unsigned short w, short h, unsigned char wst, bool uo, unsigned short a, /*unsigned char sw,*/ short px, short py, short up, short ui, unsigned char b, unsigned char t, char g)
{
    if(!tileConstructorAllowed)
        return;
    road=r;
    water=w;
    height=h;
    waste=wst;
    uniton=uo;
    animalPresent=a;
    //smallWood=sw;
    x=px;
    y=py;
    unitindex=ui;
    unitplayer=up;    
    wasteMoved=false;
    bush=b;
    tree=t;
    grass=g;
}
bool tile::walkable(unit *u) //call with destination as the this tile
{
    unsigned short *allowed=getanimalPresent(*u);
    if(!allowed) //checks all types of illegality, i.e. not being yourself, or not being able to see the tile, etc. 
    	return false;
    else
    	delete allowed;
    if(water>2) //not very shallow water
        return false; //nope
    if(animalPresent!=0) //animal on that tile
        return false; //nope
    if(uniton) //there is already a unit there, assuming all units are 1x1 tiles
        return false;
    if(tree>0)
        return false; //cannot walk on trees
    if(map[u->y][u->x].height>height+1) //you can only tolerate a height difference of 1. Maybe this should be increased if height is to be more gradually changing. Also, you can't climb up things easily, but you can fall.
        return false;
   // if(origHeight<height-1) //ditto in the other direction
   //   return false;
    if(abs(u->x-x)>1 || abs(u->y-y)>1) //if this tile is more than 1 away vertically or horizontally
        return false; 
    for(unsigned int i=0; i<allObjects.size(); i++)
    {
        if(!allObjectDesc[allObjects[i]->whatIsIt].walkable)
            return false;
    }
    return true;
}
bool tile::walkable(hiveMind* h, short fx, short fy)
{
	unsigned short *allowed=getanimalPresent(*h);
    if(!allowed) //checks for some illegality, i.e. not being yourself, or not being allowed to see the tile (even though it may be in range), etc. 
    	return false;
    else
    	delete allowed;
    if(abs(fx - h->centerx)>h->range || abs(fy - h->centery)>h->range)
        return false; //illegal
    if(abs(x - h->centerx)>h->range || abs(y - h->centery)>h->range)
        return false; //illegal
    if(water>2) //not very shallow water
        return false; //nope
    if(animalPresent!=0) //animal on that tile
        return false; //nope
    if(uniton) //there is already a unit there, assuming all units are 1x1 tiles
        return false;
    if(tree>0)
        return false; //cannot walk on trees
    if(map[fy][fx].height>height+1) //you can only tolerate a height difference of 1. Maybe this should be increased if height is to be more gradually changing. Also, you can't climb up things easily, but you can fall.
        return false;
   // if(origHeight<height-1) //ditto in the other direction
   //   return false;
    if(abs(fx-x)>1 || abs(fy-y)>1) //if this tile is more than 1 away vertically or horizontally
        return false; 
    for(unsigned int i=0; i<allObjects.size(); i++)
    {
        if(!allObjectDesc[allObjects[i]->whatIsIt].walkable)
            return false;
    }
    return true;
}
bool tile::walkable(animal *a) //call with this pointer as tile to check
{
	if(curLoops.animalIndex!=a->index) //illegal
		return false; 
    if(water>2) //not very shallow water
        return false; //nope
    if(animalPresent!=0) //animal on that tile
        return false; //nope
    if(uniton) //there is a unit there, assuming all units are 1x1 tiles
        return false;
    if(tree>0)
        return false; //cannot walk on trees
    if(map[a->y][a->x].height>height+1) //you can only tolerate a height difference of 1. Maybe this should be increased if height is to be more gradually changing. Also, you can't climb up things easily, but you can fall.
        return false;
    //if(origHeight<height-1) //ditto in the other direction
    //    return false;
    if(abs(a->x-x)>1 || abs(a->y-y)>1) //if this tile is more than 1 away vertically or horizontally
        return false; 
    for(unsigned int i=0; i<allObjects.size(); i++)
    {
        if(!allObjectDesc[allObjects[i]->whatIsIt].walkable)
            return false;
    }
    return true;
}
void tile::moveWater(int tx, int ty)
{
    if(water-height>map[ty][tx].water+1-map[ty][tx].height) //1 tolerance, no decimals. Accounts for height of underlying land
    {
        water--;
        map[ty][tx].water++; //let water flow downhill
        if(waste>0 && !wasteMoved) //move waste too!
        {
            waste--;
            map[ty][tx].waste++;
            map[ty][tx].wasteMoved=true;
        }
        if(disease.size()>0) //let any disease flow with it. Bwa ha ha!
        {
            for(unsigned int i=0; i<disease.size(); i++)
            {
                bool good=true;
                for(unsigned int j=0; j<map[ty][tx].disease.size(); j++)
                {
                    if(map[ty][tx].disease[j]==disease[i])
                    {
                        good=false;
                        break;
                    }
                }
                if(good)
                {
                    if(rand()%10000<allDiseases[disease[i]].waterSpreadability)
                    {
                        map[ty][tx].disease.push_back(disease[i]);
                        map[ty][tx].diseaseTime.push_back(diseaseTime[i]);
                    }
                }
            }
        }
    }
    if(water-height<map[ty][tx].water-1-map[ty][tx].height)
    {
        water++;
        map[ty][tx].water--;
        if(map[ty][tx].waste>0 && !map[ty][tx].wasteMoved) //move waste
        {
            map[ty][tx].waste--;
            waste++;
            wasteMoved=true;
        }
        if(map[ty][tx].disease.size()>0) //let any disease flow with it. Bwa ha ha!
        {
            for(unsigned int i=0; i<map[ty][tx].disease.size(); i++)
            {
                bool good=true;
                for(unsigned int j=0; j<disease.size(); j++)
                {
                    if(map[ty][tx].disease[i]==disease[j])
                    {
                        good=false;
                        break;
                    }
                }
                if(good)
                {
                    if(rand()%10000<allDiseases[map[ty][tx].disease[i]].waterSpreadability)
                    {
                        disease.push_back(map[ty][tx].disease[i]);
                        diseaseTime.push_back(map[ty][tx].diseaseTime[i]);
                    }
                }
            }
        }
    }
}
void tile::spreadDisease()
{
    if(frames%INFECTRATE==0)
    {
        for(unsigned int h=0; h<disease.size(); h++)
        {
            if(diseaseTime[h]>=allDiseases[disease[h]].duration)
            {
                diseaseTime.erase(diseaseTime.begin()+h); //no more disease
                disease.erase(disease.begin()+h);
                h--;
            }
            diseaseTime[h]++;
            for(int i = (y>allDiseases[disease[h]].spreadabilityArea) ? (y-allDiseases[disease[h]].spreadabilityArea) : 0; i < (MAPSIZE-y<allDiseases[disease[h]].spreadabilityArea) ? (y+allDiseases[disease[h]].spreadabilityArea) : MAPSIZE; i++)
            {
                for(int j = (x>allDiseases[disease[h]].spreadabilityArea) ? (x-allDiseases[disease[h]].spreadabilityArea) : 0; j < (MAPSIZE-x<allDiseases[disease[h]].spreadabilityArea) ? (x+allDiseases[disease[h]].spreadabilityArea) : MAPSIZE; j++)
                {
                    if(map[i][j].uniton)
                    {
                        int immunityloss=0;
                        for(unsigned int d=0; d<allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased.size(); d++)
                            immunityloss+=allDiseases[allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased[d]].immunCost;
                        if(rand()%10000<allDiseases[disease[h]].spreadabilityChance-((allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->immunity->intEval(allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->age)-immunityloss>0)?(allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->immunity->intEval(allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->age)-immunityloss):0)+((MAXHEALTH-allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->health)*allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->healthDiseaseInc))
                        {
                            bool good=true;
                            for(unsigned int d=0; d<allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased.size(); d++)
                            {
                                if(disease[h]==allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased[d].disease)
                                {
                                    good=false;
                                    allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased[d].multiplier++;
                                    allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->diseased[d].flipDir=false;
                                    break;
                                }
                            }
                            if(good)
                            {
                                allUnits.data[map[i][j].unitplayer][map[i][j].unitindex]->becomeIll(disease[h]);
                            }
                        }
                    }
                    else if(map[i][j].animalPresent>0)
					{
						if(map[i][j].x!=x || map[i][j].y!=y) //different unit
						{
							int tempimmunloss=0;
							for(unsigned int d=0; d<allAnimals[map[i][j].animalPresent-1]->diseased.size(); d++)
								tempimmunloss+=allDiseases[allAnimals[map[i][j].animalPresent-1]->diseased[d]].immunCost;
							if(rand()%10000<allDiseases[disease[h]].spreadabilityChance-((allAnimals[map[i][j].animalPresent-1]->immunity->intEval(allAnimals[map[i][j].animalPresent-1]->age)-tempimmunloss>0)?(allAnimals[map[i][j].animalPresent-1]->immunity->intEval(allAnimals[map[i][j].animalPresent-1]->age)-tempimmunloss):0)+((MAXHEALTH-allAnimals[map[i][j].animalPresent-1]->health)*allAnimals[map[i][j].animalPresent-1]->healthDiseaseInc))
							{
								bool good=true;
								for(unsigned int d=0; d<allAnimals[map[i][j].animalPresent-1]->diseased.size(); d++)
								{
									if(disease[h]==allAnimals[map[i][j].animalPresent-1]->diseased[d]) //you actually managed to get sick with the same disease TWICE! Good for you!
									{
										good=false;
										allAnimals[map[i][j].animalPresent-1]->diseased[d].multiplier++; //makes the disease more fearsome. Or at least more advanced in development
										allAnimals[map[i][j].animalPresent-1]->diseased[d].flipDir=false; //If you were getting better, screw that! Now you're getting sicker again!
										break;
									}
								}
								if(good)
								{
									allAnimals[map[i][j].animalPresent-1]->becomeIll(disease[h]);
								}
							}
						}
					}
                }
            }
        }
    }
}
bool* tile::blocksVision(unit *u)
{
    if(u->player!=curLoops.unitPlayer || u->index!=curLoops.unitIndex)
        return NULL; //cheaters. Thou fail.
    if(abs(u->y-y) > u->lineOfSight || abs(u->x-x) > u->lineOfSight) //too far to see the potential obstacle
    	return NULL;
    bool *good = new bool;
    *good = (bush>20 || tree>0);
    if(*good)
    {
    	unsigned char sightVal = (*u->currSeen)[u->lineOfSight-y+u->y][u->lineOfSight-x+u->x].get(u);
        if(sightVal==1)
        {
        	if(uniton || (animalPresent>0 && allAnimals[animalPresent-1]->animalType!=0 && allAnimals[animalPresent-1]->animalType!=3))
        		*good=false;
        }
        else if(sightVal==3)
        	*good=false;
            
    }
    return good;
}
bool* tile::blocksVision(animal *a)
{
	if(a->index!=curLoops.animalIndex)
		return NULL;
	if(abs(a->y-y) > a->lineOfSight || abs(a->x-x) > a->lineOfSight) //too far to see the potential obstacle
    	return NULL;
    bool *good = new bool;
    *good = (bush>20 || tree>0);
    if(*good)
    {
        unsigned char sightVal = (*a->currSeen)[a->lineOfSight-y+a->y][a->lineOfSight-x+a->x].get(a);
        if(sightVal==1)
        {
        	if(uniton || (animalPresent>0 && allAnimals[animalPresent-1]->animalType!=0 && allAnimals[animalPresent-1]->animalType!=3))
        		*good=false;
        }
        else if(sightVal==3)
        	*good=false;
    }
    return good;
}
tile::~tile()
{
    for(unsigned int i=0; i<allObjects.size(); i++)
    {
        delete allObjects[i];
        allObjects[i]=NULL;
    }
    allObjects.clear();
}
short tile::cameouflageAmnt()
{
	int ret = ((bush>=125) ? 124 : bush) / 25 * CAMEOPER25BUSH;
	ret+=((tree>0)?1:0) * CAMEOFORTREE;
	ret+=((road>0)?1:0) * CAMEOFORROAD;
	return ret;
}

//getters
#define X(type, val) \
    type * tile::get ## val(unit& u) \
    { \
        if(u.player!=curLoops.unitPlayer || u.index!=curLoops.unitIndex) \
            return NULL; \
        if(abs(u.y-y)>u.lineOfSight || abs(u.x-x)>u.lineOfSight) \
        	return NULL; \
        if((*u.currSeen)[u.lineOfSight+y-u.y][u.lineOfSight+x-u.x].get(&u)>0) \
        { \
            type *ret = new type; \
            *ret=val; \
            return ret; \
        } \
        return NULL; \
    } \
    type * tile::get ## val(hiveMind& h) \
    { \
        if(h.player!=curLoops.hivePlayer || h.index!=curLoops.hiveIndex) \
            return NULL; \
        if(mapseenhive[h.player][h.index][y][x].get(&h)>0) \
        { \
            type *ret = new type; \
            *ret=val; \
            return ret; \
        } \
        return NULL; \
    }
    LISTVARSTILE
#undef X

vector<object*>* tile::getallObjects(unit& u) 
{ 
    if(u.player!=curLoops.unitPlayer || u.index!=curLoops.unitIndex) \
        return NULL; \
	if(abs(u.y-y)>u.lineOfSight || abs(u.x-x)>u.lineOfSight) \
		return NULL; \
    if((*u.currSeen)[u.lineOfSight+y-u.y][u.lineOfSight+x-u.x].get(&u)>0) 
    { 
        vector<object*> *ret = new vector<object*>; 
        for(unsigned int i=0; i<allObjects.size(); i++)
        {
        	object* o = new object(*allObjects[i]);
        	ret->push_back(o);
        }
            
        return ret; 
    } 
    return NULL; 
}

vector<object*>* tile::getallObjects(hiveMind& h) 
{ 
    if(h.player!=curLoops.hivePlayer || h.index!=curLoops.hiveIndex) \
        return NULL; \
    if(mapseenhive[h.player][h.index][y][x].get(&h)>0) 
    { 
        vector<object*> *ret = new vector<object*>; 
        for(unsigned int i=0; i<allObjects.size(); i++)
        {
        	object* o = new object(*allObjects[i]);
        	ret->push_back(o);
        }
        return ret; 
    } 
    return NULL; 
} 

#define X(type, val) \
    type * tile::get ## val(unit& u) \
    { \
        if(u.player!=curLoops.unitPlayer || u.index!=curLoops.unitIndex) \
            return NULL; \
        if(abs(u.y-y)>u.lineOfSight || abs(u.x-x)>u.lineOfSight) \
			return NULL; \
		type *ret = new type; \
        if((*u.currSeen)[u.lineOfSight+y-u.y][u.lineOfSight+x-u.x].get(&u)==1) \
        { \
            *ret=val; \
            return ret; \
        } \
        if(sizeof(type)==sizeof(bool)) \
            *ret=false; \
        else \
            *ret=-1; \
        return ret; \
    } \
    type * tile::get ## val(hiveMind& h) \
    { \
        if(h.player!=curLoops.hivePlayer || h.index!=curLoops.hiveIndex) \
            return NULL; \
        type *ret = new type; \
        if(mapseenhive[h.player][h.index][y][x].get(&h)==1) \
        { \
            *ret=val; \
            return ret; \
        } \
        if(sizeof(type)==sizeof(bool)) \
            *ret=false; \
        else \
            *ret=-1; \
        return ret; \
    }
    LISTVARSTILECREATURE
#undef X

/*pubTile* tile::get(unit& u)
{
    if(mapseenunit[u.player][y][x].get()>0)
        return new pubTile(this);
    return NULL;
}
pubTile* tile::get(hiveMind& h)
{
    if(mapseenhive[h.player][h.index][y][x].get()>0)
        return new pubTile(this);
    return NULL;
}*/
