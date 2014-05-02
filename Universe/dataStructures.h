#ifndef DATASTRUCTURES_H
#define	DATASTRUCTURES_H

#include <vector>
#include "object.h"

using std::vector;

class tile;

class diseaseInfo
{
public:
    int disease;
    int time;
    int multiplier; //This is how much to multiply the costs of the disease by.
    bool flipDir; //started decreasing multiplier. true=yes. 
    diseaseInfo(int d, int t=0, int m=1);
    operator int();
};

class point
{
public:
    int x;
    int y;
    point(int px, int py);
};

class pointRad : public point
{
public:
    int rad;
    pointRad(int px, int py, int pr);
};

class unitChangeLog
{
private:
    int xorig; //id
    int yorig; //id
    int player;
    int index;
    int xmod;
    int ymod;
    int healthmod;
    int energymod;
    int hungermod;
    int sleepmod;
    int pregnantmod;
    vector<point> *sight;
public:
    unitChangeLog(int xo, int yo, int p, int i, int xm, int ym, int hm, int em, int hunm, int sm, int pm, vector<point> *s);
    static void update(int xo, int yo, int p, int i, int xm, int ym, int hm, int em, int hunm, int sm, int pm, vector<point> *s); //comment the body of this for no gui.
    static void communicate();
};

class animalChangeLog
{
private:
    int xorig; //id
    int yorig; //id
    int index;
    int xmod;
    int ymod;
    int healthmod;
    int hungermod;
    int sleepmod;
public:
    animalChangeLog(int xo, int yo, int i, int xm, int ym, int hm, int hunm, int sm);
    static void update(int xo, int yo, int i, int xm, int ym, int hm, int hunm, int sm); //comment the body of this for no gui.
    static void communicate();
};

class visionObstacle : public point
{
public:
	double slope1;
	double slope2;
	visionObstacle(int px, int py);
};

/*class pubTile
{
public:
    unsigned char road; //0=none, after that its how good the road is. 0-7 inclusive - 4 bits
    unsigned short water; //0=none, after that depth. Can get very deep. Starts at height and goes down
    short height; //can get very high or very deep
    unsigned char waste; //I think you can have multiple bits of waste on one tile. At least up to 3 inclusive. So 3 bits.
    bool uniton; // 1 bit
    unsigned short animal; //no=0, if there's an animal, its the index of that animal in whatever vector we have storing all of the animals. Animals have lots of different properties, so there will probably be an animal class at some point.
    //rightmost 4 bits (&15) amount of wood lying around. leftmost 4 bits (>>4), amount of wood that can be retrieved from present bush. If reaches 0, bush set to 0
    //unsigned char smallWood; //not a tree, but just small branches lying around. You can't use them to build a house, but you can build a lean-to or a fire. number=amount
    short unitplayer;
    short unitindex;
    unsigned char bush; //7 bits. 0=none. 1=yes, no food. 2-127 - amount of food on the bush + 1. Can use last bit for something else.
    unsigned char tree; //7 bits. 0=none. 1-127 - amount of wood ungathered from tree. As you cut wood from it, decreases to 0. If nonzero, not a walkable tile.        
    short x;
    short y;
    vector<short> disease; //index of a disease on this tile. -1 if none. If there is a disease on this tile, it is stored in water or waste. A disease in an animal or in a person will be stored separately
    vector<short> diseaseTime; //how long a disease has been on this tile. 
    vector<object> allObjects;
    pubTile(vector<short> d, vector<short> dt, vector<object> ao, unsigned char r=0, unsigned short w=0, short h=0, unsigned char wst=0, bool uo=false, unsigned short a=0, /unsigned char sw=0,* short px=0, short py=0, short up=-1, short ui=-1, unsigned char b=0, unsigned  char t=0);
    pubTile(tile* t);
};*/

#endif	/* DATASTRUCTURES_H */

