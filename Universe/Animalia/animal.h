#ifndef ANIMAL_H
#define	ANIMAL_H

#include <vector>
#include "../dataStructures.h"
#include "../metabool.h"
#include "../currLoopVar.h"

#define AMNTANIMALSIZES 3

class animalEatingAbilities;
class herbivore;
class carnivore;
class tile;

using namespace std;

#define LISTVARSANIMAL \
	X(short, health) \
	X(char, age) \
	X(bool, gender) \
	X(short, x) \
	X(short, y) \
	X(short, speed) \
	X(short, hunger) \
	X(short, index) \
	X(short, sleep) \
	X(short, weight) \
	X(short, lineOfSight) \
    X(short, animalType)

class animal
{
	friend class tile;
	friend class metabool;
    friend class herbivore;
    friend class carnivore;
    friend class animalEatingAbilities;
    friend void reformatAnimals();
    friend int main();
private:
	short health;
	char age;
	bool gender;
	short x;
	short y;
	short speed;
	short hunger;
	short index;
	short sleep;
	short weight;	
	short lineOfSight;
    short animalType; //0 is small herbivore - rabbit, 1 is medium herbivore - deer, 2 is big herbivore - giraffe/elephant/hippo, 3 is small carnivore - fox, cat, 4 is medium carnivore - wolf, bobcat, 5 is large carnivore - lion, tiger
    animalEatingAbilities *eatingBehavior;
	
	short moveToX;
	short moveToY;
	char searchDist;
	char searchDir; 
	vector<vector<metabool> > *currSeen;
	
	bool moving;
	int movingprog;
	vector<point> foodapprox;
public:
	animal();
private:
#define X(type, val) \
	type p_ ## val,
	
	animal(LISTVARSANIMAL animalEatingAbilities *p_eatingBehavior);
#undef X
protected:
#define X(type, val) \
	type get ## val(animal *a);
	LISTVARSANIMAL
#undef X
	vector<point> *getfoodapprox(animal *a);
	vector<vector<metabool> > *getcurrSeen(animal *a);
private:
	bool nextFrame();
	void livingEvents();
	bool checkLive();
	vector<vector<short> > *searchFood();
	//bool searchFoodHelperBushes(vector<vector<short> > * foodLocs);
	void move();
	void moveHelper(int mx, int my);
	void see(); //fills currSeen
	void die();
};

#endif	/* ANIMAL_H */

