#ifndef UNIT_H
#define	UNIT_H

#include <vector>
#include "dataStructures.h"
#include "object.h"
#include "throw.h"
#include "commWrapper.h"

using namespace std;

class tile;

#define NUMSKILLS 1

#define LISTVARSUNIT \
    Y(int, player) \
    Y(int, index) \
    Y(short, strength) \
    Y(bool, gender) \
    Y(short, intelligence) \
    Y(char, age) \
    Y(short, x) \
    Y(short, y) \
    Y(short, speed) \
    Y(short, lineOfSight) \
    Y(short, sexuallyMature) \
    Y(short, moveToX) \
    Y(short, moveToY) \
    Y(short, health) \
    Y(short, hunger) \
    Y(short, pregnant) \
    Y(short, fetusid) \
    Y(short, sleep) \
    Y(short, energy) \
    Y(bool, sleeping) \
    Y(short, weight) \
    Y(short, excreteNeed)

#define LISTVARSUNITSEENBYOTHER \
    Z(int, player) \
    Z(int, index) \
    Z(short, strength) \
    Z(bool, gender) \
    Z(char, age) \
    Z(short, x) \
    Z(short, y) \
    Z(short, sexuallyMature) \
    Z(short, health) \
    Z(short, pregnant) \
    Z(bool, sleeping) \
    Z(short, weight) 

class unit
{
friend class tile; //a tile can access anything in here. No need for the universe to hide data from itself
friend class allunits; //same
friend class metabool;
friend class hiveMind; //this is ok since hivemind and unit are to be inherited from anyway
friend class allminds;
friend int main();
friend void reformat();
friend class object;
friend class Throwing;
friend class commWrapper;
private: // all this stuff can only be changed internally
    //more will certainly be added. Our own creatures will inherit from this, and implement some sort of run function, probably
    int player;
    int index; //in whatever list of units there will be
    short strength; //affects how much it can carry, how well it can fight, etc. //genetic
    bool gender; //I assume 2 genders. true=male  //genetic
    short intelligence; //effects various tasks. //genetic 
    char age;
    short x;
    short y;
    short speed; //how often movement occurs. Can't be 1:1 with ticks, since vehicles have to be faster
    short lineOfSight; //how far it can see. Elevation increases? Buildings/obstacles block sight
    short immunity; //changes with age. small->large->small. Out of 10,000 //genetic
    short healthDiseaseInc; //how much the chances of disease increase per health lost. //genetic
    short woundEnergyCost; //how much energy losing health consumes, per health lost. // genetic
    short energyPerFood; //how much energy is obtained per increase of hunger by one. //genetic
    short metabolicRate; //how quickly hunger rises and energy is consumed. //genetic
    short maxMetabolicRate; //if there is little or no energy, metabolism rises to this rate. At this point, even if the body has food, it cannot gain any more energy than it is currently gaining. This makes it impossible to avoid death by eating a lot. //genetic
    short sexuallyMature; //at what age reproduction is possible. Can vary slightly. //genetic
    short weight; //pretty self-explanatory
    short fatToWeight; //how much fatBuildProgress has to advance to transform all of it into another point of weight. genetic.
    short fatRetrievalEfficiency; //how well can energy be retrieved from fat, as a fraction of fatToWeight. Out of 1000, i.e. 900 is 90% efficiency. genetic.
    short excreteNeedMax; //If you get to this point, you WILL excrete. Wherever you are. //genetic

    //below variables are not passed to constructor
    short moveToX;
    short moveToY;
    vector<diseaseInfo> diseased; //info about all of the diseases this unit has, if any
    short health; //starts at max. If wounded, decreased.
    short hunger; //starts at min. If reaches max, you die. As it rises, energy is obtained at a metabolic rate. 
    short pregnant; //-1 if no. Otherwise its how long its been.
    short fetusid; //id of child if pregnant
    short sleep; //how well-rested it is. At some point sleep deprivation starts taking effect
    short energy; //how much energy it has. Replenished by eating. Taken up by living, moving, fighting, etc.
    vector<object*> carrying;
    Throwing throwSkill;
    short *learningSkills; //one for each skill. Stores -1 if no, else stores index of teacher. Currently: only throwing.
    short fatBuildProgress; //how far you are on the way to adding weight. 
    short minWeight; //It cannot go under this. Weight will no longer be transformed to energy.
    short excreteNeed; //how much it needs to excrete. -1 is no need. Else rises to excreteNeedMax. At that point it does, wherever you happen to be.
    
    //below are 'acting' variables. If the unit is doing some action, and cannot do other actions because of this, this is where it is recorded.
    short reproducing; //0=no, else=time
    bool sleeping; //whether it is currently asleep
	int movingprog; //has to get to speed for there to be movement
    
    bool moving;  //These are cleared at the end of the frame
    bool throwing;
    bool eating;
    bool liftingOrDropping; //if pickup() or putdown() were called
    bool waking; //if just called awaken(). It takes some time. 
    bool excreting; //you can't do shit while you're excreting. 
public:
    commWrapper allCommuniques;

private:
	unit(); //use only if about to rewrite this. Should never exist on map. Nothing is initialized.
    unit(int p, int i, short str, bool g, short intel, char a, short px, short py, short pspeed, short los, short immun, short hdi, short wec, short epi, short mr, short mmr, short sm, short throwXP, short wt, short ftw, short fre, short enm);
    void diseaseEffects();
    bool checkLive();
    void livingEvents();
    void moveHelper(int mx, int my);
    void infect();
    void seeunit();
	vector<point> seeunit(bool gui);
    void unseeunit();
    void seehive(int hiveIndex);
    void unseehive(int hiveIndex);
    bool nextFrame();
    void giveBirth();
    void emergencySleep();
    void die();
    void hitWithFlyingObject(int objIndex); //Add to this function
    void resetActions();
    void resetSkills();
    void learn();
    void shit();
public:
    virtual ~unit();
    unit& operator=(const unit &source);
    void move(); //no obstacle avoidance: each creature will implement that on its own. This just moves in the direction of a target. Very simple.
    void move(short mx, short my); //no obstacle avoidance: each creature will implement that on its own. This just moves to the given square, if that's legal
    void reproduce(int withwhom);
    void goToSleep();
    void awaken();
    void pickUp(int what, int ox, int oy);
    void putDown(int objIndex, int px, int py);
    void eat(int objIndex);
    void throwObj(int objIndex, short atX, short atY, int strength);
    void learnSkillFrom(int learnwhat, short fromwhom); //Currently the only skill is throwing, but there will be more, which is what the first parameter is for
    void stopLearnSkillFrom(int learnwhat);
    void excrete();
    virtual void act(); //each person will make a class that inherits from unit. act will be overridden with AI. In this class, it should be empty
    //getters
    vector<object> getcarrying();
#define Y(type, val) \
    type get ## val() ;
    LISTVARSUNIT
#undef Y
    //getters for hivemind
    
#ifndef LISTVARSHIVE
    #define LISTVARSHIVE \
        X(int, centerx) \
        X(int, centery) \
        X(int, range) \
        X(int, player) \
        X(int, index)
#endif
    
#define X(type, val) \
    type getHiveMind ## val(int hiveIndex) ;
    LISTVARSHIVE
#undef X
    //getters for when someone else sees this unit
#define Z(type,val) \
    type getNonSelf ## val(unit* looking) ;
    LISTVARSUNITSEENBYOTHER
#undef Z
};

#endif	/* UNIT_H */

