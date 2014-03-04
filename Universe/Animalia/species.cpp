#include "species.h"
#include "animalEatingAbilities.h"

species::species(short h, short s, short los, short at, animalEatingAbilities *eb)
{
    health=h;
    speed=s;
    lineOfSight=los;
    animalType=at;
    eatingBehavior = eb;
}