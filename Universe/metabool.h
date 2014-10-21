#ifndef METABOOL_H
#define	METABOOL_H

#include "unit.h"
#include "hivemind.h"

class point;
class animal;

class metabool
{
private:
    unsigned char b; //0=false, 1=true, 2=true for landscape but not units, 3 = You saw that there is something there but you don't know what it is
public:
    metabool();
    unsigned char get(unit *u);
    unsigned char get(hiveMind *h);
    unsigned char get(animal *a);
    friend void unit::seehive(int);
    friend void unit::unseehive(int);
    friend void unit::unseeunit();
    friend class animal;
    friend class herbivore;
    friend class carnivore;
    friend class tile;
    friend class creature;
	//friend bool* tile::blocksVision(animal*);
};

#endif	/* METABOOL_H */

