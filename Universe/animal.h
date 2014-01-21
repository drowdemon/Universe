#ifndef ANIMAL_H
#define	ANIMAL_H

#include <vector>
#include "dataStructures.h"
#include "metabool.h"
#include "currLoopVar.h"

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
	X(short, lineOfSight)

class animal
{
	friend class tile;
	friend class metabool;
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
	
	animal(LISTVARSANIMAL bool extraneous=true);
#undef X
protected:
#define X(type, val) \
	type get ## val(animal *a);
	LISTVARSANIMAL
#undef X
	vector<point> *getfoodapprox(animal *a);
	vector<vector<metabool> > *getcurrSeen(animal *a);
private:
	void nextFrame();
	void livingEvents();
	bool checkLive();
	vector<vector<short> > *searchFood();
	bool searchFoodHelperBushes(vector<vector<short> > * foodLocs);
	void move();
	void moveHelper(int mx, int my);
	void see(); //fills currSeen
	void die();
};

#endif	/* ANIMAL_H */

