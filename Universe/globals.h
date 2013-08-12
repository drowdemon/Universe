#ifndef GLOBALS_H
#define	GLOBALS_H

#include <vector>
#include "tile.h"
#include "unit.h"
#include "disease.h"
#include "metabool.h"
#include "hivemind.h"
#include "dataStructures.h"
#include <fstream>
#include "allunits.h"
#include "allminds.h"
#include "currLoopVar.h"
#include "object.h"

using namespace std;

//definitions
#define WATERFLOWRATE 25
#define MAPSIZE 200
#define INFECTRATE 10
#define MAXHEALTH 100
#define MOVEMENTENERGY 5
#define LIVINGENERGY 1
#define FALLINGMULTIPLIER 0.5 //when you fall, the distance fallen * this = damage done
#define MAXHUNGER 1000
#define ENERGYCRITPOINT 100 //if energy falls below this, maxmetabolicrate is used
#define NUMPLAYERS 2
#define GESTATIONPERIOD 5000 //arbitrary, measured in system ticks. No clue how long this is
#define PREGNANTENERGYCOST 2 //every 1000 units of time pregnant, this is multiplied. So 2,4,6,8,10
#define REPRODUCTIONENERGYCOST  20
#define NEWBORNENERGY 200 //I really don't know how much this is
#define NEWBORNSLEEP  400
#define NEWBORNHUNGER 200
#define BIRTHHEALTHLOSS  MAXHEALTH*0.1
#define BIRTHENERGYLOSS  100
#define EMERGENCYSLEEPLVL 10 //when it will fall asleep when out of hive mind control
#define EMERGENCYENDSLEEP 700 //when it will awaken when sleeping as above and out of hive mind control
//function declarations
int geneMixer(int p1, int p2);

//variable declarations
extern vector<vector<tile> > map;
extern allunits allUnits;
extern unsigned long long frames;
extern vector<disease> allDiseases;
extern vector<vector<vector<metabool> > > mapseenunit; //[player][y][x]
extern vector<vector<vector<vector<metabool> > > > mapseenhive; //[player][hive][y][x]
extern allminds allMinds;
extern vector<unitChangeLog> allUnitChanges;
extern ofstream *unitChangeFile;
extern curLoopVar curLoops;
extern vector<objectDescriptor> allObjectDesc;

#endif	/* GLOBALS_H */