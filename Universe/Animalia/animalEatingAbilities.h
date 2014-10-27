#ifndef ANIMALEATING_H
#define	ANIMALEATING_H

#include "animal.h"
#include <vector>

using namespace std;

class animalEatingAbilities
{
    friend class herbivore;
    friend class carnivore;
    friend class animal;
    short animalType;
    short speciesIndex;
public:
    animalEatingAbilities(short p_animalType, short p_speciesIndex);
    virtual ~animalEatingAbilities();
private:
    virtual bool eatingBehavior(vector<vector<short> > * foodLocs, vector<vector<metabool> >* currSeen, short x, short y, short los);
    virtual bool canEat(vector<vector<short> > * foodLocs, short px, short py, short x, short y, short los) = 0;
    virtual vector<int>* eat(int what, short& hunger, vector<object*>& carrying, short x, short y); //>=3 = object on tile, 2=bush, 1=grass, <=0 = carried object
};

class herbivore : public animalEatingAbilities
{
private:
    bool eatGrass;
public:
    herbivore(short aT, short sI, bool eG);
private:
    bool canEat(vector<vector<short> > * foodLocs, short px, short py, short x, short y, short los);
    vector<int>* eat(int what, short& hunger, vector<object*>& carrying, short x, short y); //>=3 = object on tile, 2=bush, 1=grass, <=0 = carried object
};

class carnivore : public animalEatingAbilities
{
public:
	carnivore(short aT, short sI);
private:
    bool canEat(vector<vector<short> > * foodLocs, short px, short py, short x, short y, short los);
    vector<int>* eat(int what, short& hunger, vector<object*>& carrying, short x, short y); //>=3 = object on tile, 2=bush, 1=grass, <=0 = carried object, 2 and 1 not allowed.
};

#endif

