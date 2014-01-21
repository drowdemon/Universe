/* 
 * File:   currLoopVar.h
 * Author: marc
 *
 * Created on August 9, 2013, 9:49 PM
 */

#ifndef CURRLOOPVAR_H
#define	CURRLOOPVAR_H

class curLoopVar
{
    friend int main();
    friend class allunits;
    friend class allminds;
    friend class hiveMind;
    friend class unit;
    friend class tile;
    friend class object;
    friend class Throwing;
    friend class commWrapper;
	friend class metabool;
	friend class animal;
private:
    int hivePlayer;
    int hiveIndex;
    int unitPlayer;
    int unitIndex;
	int animalIndex;
	static bool created;
public:
    curLoopVar();
};

#endif	/* CURRLOOPVAR_H */

