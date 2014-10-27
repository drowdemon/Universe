#include "animalEatingAbilities.h"
#include "../globals.h"
#include "animal.h"


using namespace std;

animalEatingAbilities::animalEatingAbilities(short p_animalType, short p_speciesIndex)
{
	animalType=p_animalType;
	speciesIndex=p_speciesIndex;
}

animalEatingAbilities::~animalEatingAbilities()
{
}

bool animalEatingAbilities::eatingBehavior(vector<vector<short> > * foodLocs, vector<vector<metabool> >* currSeen, short x, short y, short los)
{
	bool ret=false;
    for(unsigned int i=0; i<currSeen->size(); i++)
    {
        for(unsigned int j=0; j<currSeen->size(); j++)
        {
            if((*currSeen)[i][j].b!=0) //can see this tile
            {
            	ret = canEat(foodLocs, j, i, x, y, los);
            }
        }
    }
    return ret;
}

vector<int>* animalEatingAbilities::eat(int what, short& hunger, vector<object*>& carrying, short x, short y) //>=3 = object on tile, 2=bush, 1=grass, <=0 = carried object
{
	vector<int>* ret = NULL;
	if(what==2) //bush
	{
		if(map[y][x].bush>1)
		{
			map[y][x].bush--;
			hunger-=allSpecies[speciesIndex].nutritionPerBush;
		}
	}
	else if(what==1) //grass
	{
		if(map[y][x].grass>0)
		{
			map[y][x].grass--;
			hunger-=allSpecies[speciesIndex].nutritionPerGrass;
		}
	}
	else if(what<=0) //carried object
	{
		what=-what; //negate, now it's the correct index
		
		//TODO maybe make this happen for each disease with spreadabilityChance probability?
		ret = new vector<int>(carrying[what]->infected);
		
		if(carrying[what]->actuallyEdible>=0)
			hunger-=carrying[what]->possFood->nutrition;
		
		//ate the object, it's gone now.
		delete carrying[what];
		carrying[what]=NULL;
		carrying.erase(carrying.begin()+what);
	}
	else if(what>=3) //object on ground
	{
		what-=3; //now it's the correct index
		
		if(map[y][x].allObjects[what]->height < map[y][x].height) //can't see it
			return NULL;
		
		//TODO maybe make this happen for each disease with spreadabilityChance probability?
		ret = new vector<int>(map[y][x].allObjects[what]->infected);
		
		if(map[y][x].allObjects[what]->actuallyEdible>=0)
			hunger-=map[y][x].allObjects[what]->possFood->nutrition;
		
		//ate the object, it's gone now.
		delete map[y][x].allObjects[what];
		map[y][x].allObjects[what]=NULL;
		map[y][x].allObjects.erase(map[y][x].allObjects.begin()+what);
	}
	
	if(hunger<0)
		hunger=0; //not entirely accurate, but there is some physical limit to how much you can eat. Here, there is no limit, but eventually it doesn't do anything when you eat.
	return ret;
}



herbivore::herbivore(short aT, short sI, bool eG) : animalEatingAbilities(aT, sI)
{
    eatGrass=eG;
}

bool herbivore::canEat(vector<vector<short> > * foodLocs, short px, short py, short x, short y, short los)
{
	tile *curTile = &map[y+py-los][x+px-los];
	
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

vector<int>* herbivore::eat(int what, short& hunger, vector<object*>& carrying, short x, short y) //>=3 = object on tile, 2=bush, 1=grass, <=0 = carried object
{
	if((what<0 && carrying[-what]->actuallyEdible!=-3 && !carrying[-what]->possFood->vegetarian) || (what>=3 && map[y][x].allObjects[what-3]->actuallyEdible!=-3 && !map[y][x].allObjects[what-3]->possFood->vegetarian))
		return NULL;
	return animalEatingAbilities::eat(what, hunger, carrying, x, y);
}


carnivore::carnivore(short aT, short sI) : animalEatingAbilities(aT, sI)
{
}

bool carnivore::canEat(vector<vector<short> >* foodLocs, short px, short py, short x, short y, short los)
{
	tile *curTile = &map[y+py-los][x+px-los];
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

vector<int>* carnivore::eat(int what, short& hunger, vector<object*>& carrying, short x, short y) //>=3 = object on tile, 2=bush, 1=grass, <=0 = carried object
{
	if(what==1 || what==2 || (what<0 && carrying[-what]->actuallyEdible!=-3 && carrying[-what]->possFood->vegetarian) || (what>=3 && map[y][x].allObjects[what-3]->actuallyEdible!=-3 && map[y][x].allObjects[what-3]->possFood->vegetarian))
		return NULL;
	return animalEatingAbilities::eat(what, hunger, carrying, x, y);
}
