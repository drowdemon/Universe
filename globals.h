#ifndef GLOBALS_H
#define	GLOBALS_H

#include <vector>
#include "tile.h"
#include "unit.h"

using namespace std;

#define WATERFLOWRATE 25
#define MAPSIZE 200

extern vector<vector<tile> > map;
extern vector<vector<unit> > allUnits;
extern unsigned long long frames;

#endif	/* GLOBALS_H */

