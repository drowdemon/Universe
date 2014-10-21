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
    virtual bool eatingBehavior(vector<vector<short> > * foodLocs);
    virtual bool canEat(vector<vector<short> > * foodLocs, short px, short py) = 0;
    virtual ~animalEatingAbilities();
};

class herbivore : animalEatingAbilities
{
public:
    bool eatGrass;
    herbivore(bool eG);
    bool canEat(vector<vector<short> > * foodLocs, short px, short py);
};

class carnivore : animalEatingAbilities
{
public:
    bool canEat(vector<vector<short> > * foodLocs, short px, short py);
};

#endif

