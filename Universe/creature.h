#ifndef CREATURE_H
#define	CREATURE_H

#include <vector>

using namespace std;

#define LISTVARSCREATURE      \
    W(bool, gender)	          \
    W(short, speed)	          \
    W(int, index)	          \
    W(short, lineOfSight)     \
    W(short, health)	      \
    W(short, weight)	      \
    W(short, hunger)	      \
    W(short, x)		          \
    W(short, y)		          \
    W(short, sleep)	          \
    W(char, age)	          \
    W(short, energy)          \
    W(short, pregnant)        \
    W(short, speciesIndex)

#define LISTVARSCREATURECONSTRUCTORONLY \
	W(short, woundEnergyCost) \
	W(short, minWeight) \
	W(short, fatToWeight) \
	W(short, fatRetrievalEfficiency) \
	W(short, maxMetabolicRate) \
	W(short, energyPerFood) \
	W(short, metabolicRate) \

class metabool;

class creature
{
protected:
	//vary from animal to animal, but constant within animal 
	bool gender; //I assume 2 genders. true=male  //genetic
	short speed; //how often movement occurs. Can't be 1:1 with ticks, since vehicles have to be faster
	int index; //in whatever list of units/animals/creatures there will be
	short lineOfSight; //how far it can see. Elevation increases? Buildings/obstacles block sight
	short woundEnergyCost; //how much energy losing health consumes, per health lost. // genetic
	short minWeight; //It cannot go under this. Weight will no longer be transformed to energy.
	short fatToWeight; //how much fatBuildProgress has to advance to transform all of it into another point of weight. genetic.
	short fatRetrievalEfficiency; //how well can energy be retrieved from fat, as a fraction of fatToWeight. Out of 1000, i.e. 900 is 90% efficiency. genetic.
	short maxMetabolicRate; //if there is little or no energy, metabolism rises to this rate. At this point, even if the body has food, it cannot gain any more energy than it is currently gaining. This makes it impossible to avoid death by eating a lot. //genetic
	short energyPerFood; //how much energy is obtained per increase of hunger by one. //genetic
	short metabolicRate; //how quickly hunger rises and energy is consumed. //genetic
	short speciesIndex; 
	
	//vary within animal (from here on out)
	short health; //starts at max. If wounded, decreased.
	short weight; //pretty self-explanatory
	short hunger; //starts at min. If reaches max, you die. As it rises, energy is obtained at a metabolic rate.
	short x;      
	short y;
	short sleep;  //how well-rested it is. At some point sleep deprivation starts taking effect
	char age;
	short energy; //how much energy it has. Replenished by eating. Taken up by living, moving, fighting, etc.s
	short pregnant; //-1 if no. Otherwise its how long its been.s
	short fatBuildProgress; //how far you are on the way to adding weight.
	
	//not in #define, from here on out.
	short moveToX;
	short moveToY;
	vector<vector<metabool> > *currSeen;
	
	//acting variables. Doing action, cannot do anything else. //below are 'acting' variables. If the unit is doing some action, and cannot do other actions because of this, this is where it is recorded.
	short movingprog;  //has to get to speed for there to be movement
	bool sleeping;     //whether it is currently asleep
	short reproducing; //0=no, else=time
	
	//cleared at end of frame.
	bool moving;
	
protected:
	creature();
#define W(type, val) \
	type p_ ## val,
    
    creature(LISTVARSCREATURE LISTVARSCREATURECONSTRUCTORONLY bool extraneous=false);
#undef W
    virtual bool nextFrame()=0;
	virtual void livingEvents(int speciesIndex);
	virtual bool checkLive(short maxHunger);
	virtual void move();
	virtual void moveHelper(int mx, int my)=0;
	virtual void see(); //fills currSeen
	virtual void die()=0;
	virtual void giveBirth()=0;
	
	virtual ~creature();
};

#endif
