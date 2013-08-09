#ifndef GLOBALS_H
#define	GLOBALS_H

#include <vector>
#include "tile.h"
#include "unit.h"
#include "disease.h"

using namespace std;

#define WATERFLOWRATE 25
#define MAPSIZE 200
#define INFECTRATE 10
#define MAXHEALTH 100
#define MOVEMENTENERGY 5
#define LIVINGENERGY 1
#define FALLINGMULTIPLIER 0.5 //when you fall, the distance fallen * this = damage done

extern vector<vector<tile> > map;
extern vector<vector<unit> > allUnits;
extern unsigned long long frames;
extern vector<disease> allDiseases;

#endif	/* GLOBALS_H */

