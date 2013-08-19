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
private:
    int hivePlayer;
    int hiveIndex;
    int unitPlayer;
    int unitIndex;
public:
    curLoopVar();
};

#endif	/* CURRLOOPVAR_H */

