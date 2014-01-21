#ifndef METABOOL_H
#define	METABOOL_H

#include "unit.h"
#include "hivemind.h"

class point;
class animal;

class metabool
{
private:
    unsigned char b; //0=false, 1=true, 2=true for landscape but not units
public:
    metabool();
    unsigned char get(unit *u);
	unsigned char get(hiveMind *h);
	unsigned char get(animal *a);
    friend void unit::seeunit();
	friend vector<point> unit::seeunit(bool);
    friend void unit::seehive(int);
    friend void unit::unseehive(int);
    friend void unit::unseeunit();
	friend class animal;
	friend class tile;
	//friend bool* tile::blocksVision(animal*);
};

#endif	/* METABOOL_H */

