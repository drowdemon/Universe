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
    vector<pointRad> requestWhichUnits; //locations of units you want data about, with a +- radius
    bool requestAllUnits; //instead of whichUnits, just request data about the given subset of all units.
    boolListTile requestTile;
    vector<point> requestWhichTiles;
    
    //An offer of information, can be made in conjunction with a request for other information. The offer doesn't actually give any information, just describes what that information will be about.
    boolListUnit offerUnit; //A conditional offer of data for a unit, and what particular data about the unit is wanted. If the unit matches the conditions, the data will be given. Else, it won't. So it selects a subset of whichunits.
    vector<pointRad> offerWhichUnits; //locations of units you want data about, with a +- radius
    bool offerAllUnits; //instead of whichUnits, just request data about the given subset of all units.
    boolListTile offerTile;
    vector<point> offerWhichTiles;
    
    //giving information
    vector<unitTuple> unitInfo;
    vector<tileTuple> tileInfo;
	
	//who am I
	unit *whoAsks;
    communicationProtocol(unit *wa);
};

#endif	/* COMMPROTOCOL_H */

