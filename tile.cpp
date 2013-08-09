#include "tile.h"
#include "globals.h"
#include <cmath>

tile::tile(unsigned char r, unsigned short w, short h, unsigned char wst, bool uo, unsigned short a, unsigned char sw, short px, short py)
{
    road=r;
    water=w;
    height=h;
    waste=wst;
    uniton=uo;
    animal=a;
    smallWood=sw;
    x=px;
    y=py;
}
bool tile::walkable(short origHeight, short fx, short fy)
{
    if(water>2) //not very shallow water
        return false; //nope
    if(animal!=0) //animal on that tile
        return false; //nope
    if(uniton) //there is already a unit there, assuming all units are 1x1 tiles
        return false;
    if(origHeight>height+1) //you can only tolerate a height difference of 1. Maybe this should be increased if height is to be more gradually changing
        return false;
    if(origHeight<height-1) //ditto in the other direction
        return false;
    if(abs(fx-x)>1 || abs(fy-y)>1) //if this tile is more than 1 away vertically or horizontally
        return false; 
    return true;
}

void tile::moveWater(int tx, int ty)
{
    if(water>map[ty][tx].water+1) //1 tolerance, no decimals
    {
        water--;
        map[ty][tx].water++;
    }
    if(water<map[ty][tx].water-1)
    {
        water++;
        map[ty][tx].water--;
    }
}