#ifndef ANIMAL_H
#define	ANIMAL_H

#include <vector>
#include "../dataStructures.h"
#include "../metabool.h"
#include "../currLoopVar.h"
#include "../creature.h"

#define AMNTANIMALSIZES 3

class animalEatingAbilities;
class herbivore;
class carnivore;
class tile;

using namespace std;

#define LISTVARSANIMAL	      \
    X(short, animalType)      \
	X(short, skittish)        \

class animal : private creature //each actual animal is one of these
{
    friend class tile;
    friend class metabool;
    friend class herbivore;
    friend class carnivore;
    friend class animalEatingAbilities;
    friend class creature;
    friend void reformatAnimals();
    friend int main();
private:
    short animalType; //0 is small herbivore - rabbit, 1 is medium herbivore - deer, 2 is big herbivore - giraffe/elephant/hippo, 3 is small carnivore - fox, cat, 4 is medium carnivore - wolf, bobcat, 5 is large carnivore - lion, tiger
    short skittish; //how nervous it is when it sees something unknown
    
    char searchDist;
    char searchDir; 

    vector<point> foodapprox;
public:
    animal();
private:
#define X(type, val)				\
    type p_ ## val,
    
#define W(type, val) X(type, val)
    animal(LISTVARSCREATURE LISTVARSCREATURECONSTRUCTORONLY LISTVARSANIMAL bool extraneous=true);
#undef X
#undef W
protected:
#define X(type, val)				\
    type get ## val(animal *a);
#define W(type, val) X(type, val)
    LISTVARSCREATURE
    LISTVARSANIMAL
#undef X
#undef W
    vector<point> *getfoodapprox(animal *a);
    vector<vector<metabool> > *getcurrSeen(animal *a);
private:
    bool nextFrame();
    void livingEvents(int p_speciesIndex);
    vector<vector<short> > *searchFood();
    //bool searchFoodHelperBushes(vector<vector<short> > * foodLocs);
    void moveHelper(int mx, int my);
    void die();
    void giveBirth();
    creature* createFetus(int withwhom);
    //below are things an animal does, actions to take.
    void reproduce(int withWhom);
    void goToSleep();
    void seeIntently(short dirSee);
    void pickUp(int what, int ox, int oy); //animals can only pick up one object at a time
    void putDown(int objIndex, int px, int py);
    void eat(int what);
    void move(); //for some checking on whether this is allowed
protected:
    void act();
};

#endif	/* ANIMAL_H */

