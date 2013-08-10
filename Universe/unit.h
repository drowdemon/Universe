#ifndef UNIT_H
#define	UNIT_H

#include <vector>
#include "dataStructures.h"

using namespace std;

class tile;

class unit
{
friend class tile; //a tile can access anything in here. No need for the universe to hide data from itself
friend class allunits; //same
friend class metabool;
friend class hiveMind; //this is ok since hivemind and unit are to be inherited from anyway
friend class allminds;
friend int main();
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

    //below variables are not passed to constructor
    short moveToX;
    short moveToY;
    vector<diseaseInfo> diseased; //info about all of the diseases this unit has, if any
    short health; //starts at max. If wounded, decreased.
    short hunger; //starts at min. If reaches max, you die. As it rises, energy is obtained at a metabolic rate. 
    short pregnant; //-1 if no. Otherwise its how long its been.
    short childid; //id of child if pregnant
    short sleep; //how well-rested it is. At some point sleep deprivation starts taking effect
    short energy; //how much energy it has. Replenished by eating. Taken up by living, moving, fighting, etc.

    unit(int p, int i, short str, bool g, short intel, char a, short px, short py, short pspeed, short los, short immun, short hdi, short wec, short epi, short mr, short mmr, short sm);
    void diseaseEffects();
    bool checkLive();
    void livingCosts();
    void moveHelper(int mx, int my);
    void infect();
    void seeunit();
    void unseeunit();
    void seehive(int hiveIndex);
    void unseehive(int hiveIndex);
    bool nextFrame();
public:
    void move(); //no obstacle avoidance: each creature will implement that on its own. This just moves in the direction of a target. Very simple.
    bool reproduce(int withwhom);
    virtual void act(); //each person will make a class that inherits from unit. act will be overridden with AI
};

#endif	/* UNIT_H */

