#ifndef TILE_H
#define	TILE_H

#include <vector>
#include "unit.h"
#include "hivemind.h"
#include "dataStructures.h"
#include "object.h"

using namespace std;

class tile //default - empty grass
{   
    friend void waterFlow(int,int);
    friend class unit;
    friend void init();
    friend class vector<tile>;
    friend void printMap();
    friend int main();
    friend pubTile::pubTile(tile*);
    friend class Throwing;
    friend class object;
private: //private so that you can't just learn anything about any part of the map
    //Once we figure out all of the variables this will contain, I recommend we use some bitshifting tricks to compact this as much as possible. The map will take up lots of RAM, and compacting this will let us have more tiles.
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
    bool wasteMoved; //1 bit. Only for water tiles. If the waste in the water has moved, don't move it again this time around.
    unsigned char bush; //7 bits. 0=none. 1=yes, no food. 2-127 - amount of food on the bush + 1. Can use last bit for something else.
    unsigned char tree; //7 bits. 0=none. 1-127 - amount of wood ungathered from tree. As you cut wood from it, decreases to 0. If nonzero, not a walkable tile.        
public:
    short x;
    short y;
        
private:        
    vector<short> disease; //index of a disease on this tile. -1 if none. If there is a disease on this tile, it is stored in water or waste. A disease in an animal or in a person will be stored separately
    vector<short> diseaseTime; //how long a disease has been on this tile. 
    vector<object*> allObjects; //all of the objects on this tile
    tile(unsigned char r=0, unsigned short w=0, short h=0, unsigned char wst=0, bool uo=false, unsigned short a=0, /*unsigned char sw=0,*/ short px=0, short py=0, short up=-1, short ui=-1, unsigned char b=0, unsigned  char t=0);
    void moveWater(int tx, int ty);
    void spreadDisease();
public:
    ~tile();
    bool walkable(unit *u); //whether the given unit can walk on the tile. //vehicles will be added later //buildings will be added later
    bool walkable(hiveMind *h, short fx, short fy); //whether the given unit can walk on the tile. //vehicles will be added later //buildings will be added later
    pubTile* get(unit& u);
    pubTile* get(hiveMind& h);
};

#endif	/* TILE_H */

