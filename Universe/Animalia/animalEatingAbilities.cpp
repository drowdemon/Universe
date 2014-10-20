#include "animalEatingAbilities.h"
#include "../globals.h"
#include "animal.h"


using namespace std;

herbivore::herbivore(bool eG)
{
    eatGrass=eG;
}

bool herbivore::eatingBehavior(vector<vector<short> > * foodLocs)
{
    bool ret=false;
    for(unsigned int i=0; i<currSeen->size(); i++)
    {
        for(unsigned int j=0; j<currSeen->size(); j++)
        {
            if((*currSeen)[i][j].b!=0) //can see this tile
            {
                (*foodLocs)[i][j]=map[y+i-lineOfSight][x+j-lineOfSight].bush-1; //has food
                if(eatGrass && (*foodLocs)[i][j]==0 && map[y+i-lineOfSight][x+j-lineOfSight].road==0) //walkable and no road --> its grass or bushes, which can be eaten, and can eat grass, and didn't already say we're eating bushes
                {
                    (*foodLocs)[i][j]=1000; //random number to represent food on grass tile. It just never ends. For now. Maybe later add grass->mud. Definitely need dirt tile for farming
                }
                if((*foodLocs)[i][j]>0)
                    ret=true;
            }
        }
    }
    return ret;
}

bool carnivore::eatingBehavior(vector<vector<short> >* foodLocs)
{
    bool ret=false;
    for(unsigned int i=0; i<currSeen->size(); i++)
    {
        for(unsigned int j=0; j<currSeen->size(); j++)
        {
            if((*currSeen)[i][j].b!=0) //can see this tile
            {
                if(map[y+i-lineOfSight][x+j-lineOfSight].animalPresent>0) //there's an animal there
                {
                    if(animalType-AMNTANIMALSIZES>=allAnimals[map[y+i-lineOfSight][x+j-lineOfSight].animalPresent-1]->animalType || (animalType>=allAnimals[map[y+i-lineOfSight][x+j-lineOfSight].animalPresent-1]->animalType && allAnimals[map[y+i-lineOfSight][x+j-lineOfSight].animalPresent-1]->animalType>=AMNTANIMALSIZES)) //small enough that its edible
                    {
                        (*foodLocs)[i][j]=(((allAnimals[map[y+i-lineOfSight][x+j-lineOfSight].animalPresent-1]->animalType-AMNTANIMALSIZES>0) ? (allAnimals[map[y+i-lineOfSight][x+j-lineOfSight].animalPresent-1]->animalType-AMNTANIMALSIZES) : (allAnimals[map[y+i-lineOfSight][x+j-lineOfSight].animalPresent-1]->animalType) )+1)*ANIMALSIZETOFOODAMOUNT; //has food
                        ret=true;
                    }
                }
            }
        }
    }
    return ret;
}
