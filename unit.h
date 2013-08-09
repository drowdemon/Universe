#ifndef UNIT_H
#define	UNIT_H

#include <vector>
#include "dataStructures.h"

using namespace std;

class unit
{
public: 
    //more will certainly be added. Our own creatures will inherit from this, and implement some sort of run function, probably
    int player;
    int index; //in whatever list of units there will be
    short sleep; //how well-rested it is. At some point sleep deprivation starts taking effect
    short energy; //how much energy it has. Replenished by eating. Taken up by living, moving, fighting, etc.
    short strength; //affects how much it can carry, how well it can fight, etc. //genetic
    bool gender; //I assume 2 genders  //genetic
    short intelligence; //effects various tasks. //genetic 
    unsigned char age;
    short x;
    short y;
    short speed; //how often movement occurs. Can't be 1:1 with ticks, since vehicles have to be faster
    short lineOfSight; //how far it can see. Elevation increases? Buildings/obstacles block sight
    short immunity; //changes with age. small->large->small. Out of 10,000 //genetic
    short healthDiseaseInc; //how much the chances of disease increase per health lost. //genetic
    short woundEnergyCost; //how much energy a losing health consumes, per health lost. // genetic

    //below variables are not passed to constructor
    short moveToX;
    short moveToY;
    vector<diseaseInfo> diseased; //info about all of the diseases this unit has, if any
    short health; //starts at max. If wounded, decreased.

    unit(int p, int i, short s, short e, short str, bool g, short intel, unsigned char a, short px, short py, short pspeed, short los, short immun, short hdi, short wec);
    bool nextFrame();
    void move(); //no obstacle avoidance: each creature will implement that on its own. This just moves in the direction of a target. Very simple.
    void infect();
    void diseaseEffects();
    bool checkLive();
    void livingCosts();
};

#endif	/* UNIT_H */

