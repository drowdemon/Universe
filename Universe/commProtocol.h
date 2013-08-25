#ifndef COMMPROTOCOL_H
#define	COMMPROTOCOL_H

#include "TransBoolExpr/boolListUnit.h"
#include "TransBoolExpr/boolListTile.h"
#include <vector>
#include "dataStructures.h"

using std::vector;

class communicationProtocol
{
public:
    boolListUnit requestUnit; //A conditional request for a unit, and what particular data about the unit is wanted.
    vector<pointRad> whichUnits; //locations of units you want data about, with a +- radius
    boolListTile requestTile;
    vector<point> whichTiles;
    communicationProtocol();
};

#endif	/* COMMPROTOCOL_H */

