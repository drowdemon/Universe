#ifndef ANIMALEATING_H
#define	ANIMALEATING_H

#include "animal.h"
#include <vector>

using namespace std;

class animalEatingAbilities : animal
{
    friend class herbivore;
    friend class carnivore;
public:
    virtual bool eatingBehavior(vector<vector<short> > * foodLocs) = 0;
};

class herbivore : animalEatingAbilities
{
public:
    bool eatGrass;
    herbivore(bool eG);
    bool eatingBehavior(vector<vector<short> > * foodLocs);
};

class carnivore : animalEatingAbilities
{
public:
    bool eatingBehavior(vector<vector<short> > * foodLocs);
};

#endif

