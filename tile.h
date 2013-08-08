#ifndef TILE_H
#define	TILE_H

class tile //default - empty grass
{
public: //later we'll make some private so that you can't just learn anything about any part of the map
	//Once we figure out all of the variables this will contain, I recommend we use some bitshifting tricks to compact this as much as possible. The map will take up lots of RAM, and compacting this will let us have more tiles.
	unsigned char road; //0=none, after that its how good the road is. 0-7 inclusive - 4 bits
	unsigned short water; //0=none, after that depth. Can get very deep. Starts at height and goes down
	short height; //can get very high or very deep
	unsigned char waste; //I think you can have multiple bits of waste on one tile. At least up to 3 inclusive. So 3 bits.
	bool uniton; // 1 bit
	unsigned short animal; //no=0, if there's an animal, its the index of that animal in whatever vector we have storing all of the animals. Animals have lots of different properties, so there will probably be an animal class at some point.
	unsigned char smallWood; //not a tree, but just small branches lying around. You can't use them to build a house, but you can build a lean-to or a fire. number=amount
	short x;
	short y;
	tile(unsigned char r=0, unsigned short w=0, short h=0, unsigned char wst=0, bool uo=false, unsigned short a=0, unsigned char sw=0, short px=0, short py=0);
	bool walkable(short origHeight, short fx, short fy); //whether the given unit can walk on the tile. Argument for later compatability with animals. //vehicles will be added later //buildings will be added later
};

#endif	/* TILE_H */

