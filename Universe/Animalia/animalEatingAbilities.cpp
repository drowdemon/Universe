#include "animalEatingAbilities.h"
#include "../globals.h"
#include "animal.h"


using namespace std;

animalEatingAbilities::~animalEatingAbilities()
{
}

bool animalEatingAbilities::eatingBehavior(vector<vector<short> > * foodLocs)
{
	bool ret=false;
    for(unsigned int i=0; i<currSeen->size(); i++)
    {
        for(unsigned int j=0; j<currSeen->size(); j++)
        {
            if((*currSeen)[i][j].b!=0) //can see this tile
            {
            	ret = canEat(foodLocs, j, i);
            }
        }
    }
    return ret;
}

herbivore::herbivore(bool eG)
{
    eatGrass=eG;
}

bool herbivore::canEat(vector<vector<short> > * foodLocs, short px, short py)
{
	tile *curTile = &map[y+py-lineOfSight][x+px-lineOfSight];
	
	(*foodLocs)[py][px]=curTile->bush-1; //has food
	if((*foodLocs)[py][px]<=0)
	{
		(*foodLocs)[py][px]=0;
	}
	
	for(unsigned int k=0; k<curTile->allObjects.size(); k++)
	{
		if(allObjectDesc[curTile->allObjects[k]->whatIsIt].vaguelyEdible)
		{
			if(curTile->allObjects[k]->possFood->getVegetarian())
				(*foodLocs)[py][px] += 50; //completely arbitrary value for putting into foodLocs
		}
	}

	if(eatGrass && curTile->grass>0) //there's grass and can eat grass
	{
		(*foodLocs)[py][px] += curTile->grass; 
	}
	return ((*foodLocs)[py][px]>0);
}

bool carnivore::canEat(vector<vector<short> >* foodLocs, short px, short py)
{
	tile *curTile = &map[y+py-lineOfSight][x+px-lineOfSight];
	if(curTile->animalPresent>0) //there's an animal there
	{
		if(animalType-AMNTANIMALSIZES>=allAnimals[curTile->animalPresent-1]->animalType || (animalType>=allAnimals[curTile->animalPresent-1]->animalType && allAnimals[curTile->animalPresent-1]->animalType>=AMNTANIMALSIZES)) //small enough that its edible
			(*foodLocs)[py][px]=(((allAnimals[curTile->animalPresent-1]->animalType-AMNTANIMALSIZES>0) ? (allAnimals[curTile->animalPresent-1]->animalType-AMNTANIMALSIZES) : (allAnimals[curTile->animalPresent-1]->animalType) )+1)*ANIMALSIZETOFOODAMOUNT; //has food
	}
	
	for(unsigned int k=0; k<curTile->allObjects.size(); k++)
	{
		if(allObjectDesc[curTile->allObjects[k]->whatIsIt].vaguelyEdible)
		{
			if(!(curTile->allObjects[k]->possFood->getVegetarian()))
				(*foodLocs)[py][px] += 50; //completely arbitrary value for putting into foodLocs. You don't get to magically know how nutritious it is.
		}
	}
	return ((*foodLocs)[py][px]>0);
}
