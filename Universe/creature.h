#ifndef CREATURE_H
#define	CREATURE_H

#include "dataStructures.h"
#include "mathFunction.h"
#include <vector>

using namespace std;

#define LISTVARSCREATURE       \
    W(bool, gender)	           \
    W(mathFunction*, speed)	   \
    W(mathFunction*, strength) \
    W(int, index)	           \
    W(short, lineOfSight)      \
    W(short, health)	       \
    W(short, weight)	       \
    W(short, hunger)	       \
    W(short, x)		           \
    W(short, y)		           \
    W(short, sleep)	           \
    W(char, age)	           \
    W(short, energy)           \
    W(short, pregnant)         \
    W(short, speciesIndex)

#define LISTVARSCREATURECONSTRUCTORONLY     \
	W(mathFunction*, woundEnergyCost)       \
	W(mathFunction*, minWeight) 		    \
	W(short, fatToWeight) 				    \
	W(short, fatRetrievalEfficiency) 	    \
	W(short, maxMetabolicRate) 			    \
	W(short, energyPerFood) 			    \
	W(short, metabolicRate) 			    \
	W(short, coefOfWorseningSight) 		    \
	W(short, sexuallyMature) 			    \
	W(mathFunction*, immunity) 				\
	W(short, healthDiseaseInc) 				\
	W(unsigned long long, frameOfBirth) 	\

class metabool;

class creature
{
protected:
	//TODO create a way to store a formula based on age for a particular variable. For example, minweight might go up with age up to a certain point, and then stabilize (asymptotically maybe). Metabolism might go up sharply for some time and then gradually decline. etc. Coefficients would likely be genetic, actual function would not be.
	//vary from animal to animal, but constant within animal 
	bool gender; //I assume 2 genders. true=male  //genetic
	mathFunction* speed; //how often movement occurs. Can't be 1:1 with ticks, since vehicles have to be faster
	mathFunction* strength; //The whole function must be enclosed in a single term, so as to have an outer coefficient. The function can reside in the actOn variable of that term. //affects how much it can carry, how well it can fight, etc. //genetic
	int index; //in whatever list of units/animals/creatures there will be
	short lineOfSight; //how far it can see. Elevation increases? Buildings/obstacles block sight
	mathFunction* woundEnergyCost; //how much energy losing health consumes, per health lost. // genetic
	mathFunction* minWeight; //It cannot go under this. Weight will no longer be transformed to energy.
	short fatToWeight; //how much fatBuildProgress has to advance to transform all of it into another point of weight. genetic.
	short fatRetrievalEfficiency; //how well can energy be retrieved from fat, as a fraction of fatToWeight. Out of 1000, i.e. 900 is 90% efficiency. genetic.
	short maxMetabolicRate; //if there is little or no energy, metabolism rises to this rate. At this point, even if the body has food, it cannot gain any more energy than it is currently gaining. This makes it impossible to avoid death by eating a lot. //genetic
	short energyPerFood; //how much energy is obtained per increase of hunger by one. //genetic
	short metabolicRate; //smaller is faster //how quickly hunger rises and energy is consumed. //genetic
	short speciesIndex; 
	short coefOfWorseningSight; //There's some probability of seeing something after the line of perfect sight. That varies with distance quadratically. P = (lineOfSight-dist)^2*coef. P is out of 10000 rather than 1.  //if  lineOfSight-lineOfPerfectSight = 5, this should be around 40. When that difference is 2, maybe around 250. coef*maxValueOfQuadratic should be roughly 1000, so that the percent is roughly 10%
	short sexuallyMature; //at what age reproduction is possible. Can vary slightly. //genetic
	mathFunction* immunity; //changes with age. small->large->small. Out of 10,000. Like sight, whole function must be encapsulated into one term to have a single outer coefficient //genetic
	short healthDiseaseInc; //how much the chances of disease increase per health lost. //genetic
	unsigned long long frameOfBirth; //what frame the creature was born on.
	
	//vary within animal (from here on out)
	short health; //starts at max. If wounded, decreased.
	short weight; //pretty self-explanatory
	short hunger; //starts at min. If reaches max, you die. As it rises, energy is obtained at a metabolic rate.
	short x;      
	short y;
	short sleep;  //how well-rested it is. At some point sleep deprivation starts taking effect
	char age;
	short energy; //how much energy it has. Replenished by eating. Taken up by living, moving, fighting, etc.
	short pregnant; //-1 if no. Otherwise its how long its been.
	
	//not in #define, from here on out.
	short moveToX;
	short moveToY;
	short fetusid; //id of child if pregnant, else -1
	short fatBuildProgress; //how far you are on the way to adding weight.
	short seeDir; //The direction you are trying to look in. 0 is none, then up, down, left, right, are 1,2,3,4 respectively.
	
	//acting variables. Doing action, cannot do anything else. //below are 'acting' variables. If the unit is doing some action, and cannot do other actions because of this, this is where it is recorded.
	short movingprog;  //has to get to speed for there to be movement
	bool sleeping;     //whether it is currently asleep
	short reproducing; //0=no, else=time
	short seeingIntently; //0=no, 1=done this frame, 2=will do next frame
	
	//cleared at end of frame.
	bool moving;
	bool waking;
	bool liftingOrDropping; //if pickup() or putdown() were called
	bool eating;
	
	vector<object*> carrying;
	vector<vector<metabool> > *currSeen;
	vector<diseaseInfo> diseased; //info about all of the diseases this unit has, if any
protected:
	creature();
#define W(type, val) \
	type p_ ## val,
    
    creature(LISTVARSCREATURE LISTVARSCREATURECONSTRUCTORONLY bool extraneous=false);
#undef W
    creature(const creature &source);
    virtual bool nextFrame()=0;
	virtual void livingEvents(int speciesIndex);
	virtual bool checkLive(short maxHunger);
	virtual void move();
	virtual void moveHelper(int mx, int my)=0;
	virtual void see(); //fills currSeen
	virtual vector<point> seeGUI();
	virtual void die()=0;
	virtual void giveBirth()=0;
	virtual void goToSleep()=0;
	virtual void resetActions();
	virtual void awaken();
	virtual void reproduce(int withwhom, creature* cwith);
	virtual void reproduce(int withwhom)=0;
	virtual creature* createFetus(int withwhom)=0;
	virtual void seeIntently(short dirSee); //look more carefully, which means that anything that you saw as something, without knowing what it was that you saw, will now be seen exactly. Elimanates the probability bit of that, makes it a 100% chance of seeing anything you could possibly see.
	virtual bool sameDirSeeMove(short dirSee, short dx, short dy); //dy is checkingY-positionY. i.e. y+dy = where I'm checking. Same with x. 
	virtual void infect();
	virtual void diseaseEffects();
	virtual void hitWithFlyingObject(int objIndex); //Add to this function
	virtual void pickUp(int what, int ox, int oy);
	virtual void putDown(int objIndex, int px, int py);
	virtual void eat(int what)=0;
	virtual void becomeIll(int d);
	
	virtual ~creature();
};

#endif
