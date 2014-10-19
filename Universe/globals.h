// -*- mode: C++ -*-
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
#include "Animalia/animal.h"
#include "species.h"

using namespace std;

//definitions
#define WATERFLOWRATE 25
#define MAPSIZE 200
#define INFECTRATE 10
#define MAXHEALTH 100
#define MOVEMENTENERGY 15
#define LIVINGENERGYPERSTRENGTH 1  //muscle takes more energy to sustain. Per 5 strength.
#define FALLINGMULTIPLIER 0.5 //when you fall, the distance fallen * this = damage done
#define MAXHUNGER 1000
#define ENERGYCRITPOINT 300 //if energy falls below this, maxmetabolicrate is used
#define NUMPLAYERS 2
#define GESTATIONPERIOD 5000 //arbitrary, measured in system ticks. No clue how long this is
#define PREGNANTENERGYCOST 3 //every 1000 units of time pregnant, this is multiplied. So 3,6,9,12,15
#define REPRODUCTIONENERGYCOST  60
#define NEWBORNENERGY 600 //I really don't know how much this is
#define NEWBORNSLEEP  400
#define NEWBORNHUNGER 200
#define BIRTHHEALTHLOSS  MAXHEALTH*0.1
#define BIRTHENERGYLOSS  400
#define EMERGENCYSLEEPLVL 10 //when it will fall asleep when out of hive mind control
#define EMERGENCYENDSLEEP 700 //when it will awaken when sleeping as above and out of hive mind control
#define REPRODUCTIONTIME  5
#define WEIGHTPERSTRENGTH 2 //how much weight you can carry per strength point
#define SPEEDTODAMAGE 1 //for now, only a thrown objects speed determines its damage. Add more to that.
#define NUMDEADTOREFORMAT 10000 //control how often allUnits is defragged
#define NUMANIMALSDEADTOREFORMAT 5000 //control how often allAnimals is defragged
#define ENERGYSOFTMAX 6000 //arbitrary, as usual. After this, energy is converted to fat
#define ENERGYFROMFATPOINT 750 //totally arbitrary. After this (below this), fat is converted to energy. Hunger still increases though.
#define ENERGYFROMFATRATE 64 //again arbitrary. In frames per event. Weight transforms to fat at this rate.
#define NEWBORNMINWEIGHT 6 //how light a newborncan be, for constructor. After this, the min weight changes (deterministically) based on age, so it's a member variable.
#define MOVINGSELFWEIGHTPENALTY 5 //per 50 weight, with it ceiled or floored according to rand()%(weight%50)
#define MOVINGLIFTEDWEIGHTPENALTY 10 //per 50 weight, same procedure as above
#define EXCRETIONFREQ 100 //how much hunger has to rise for excretion to be required.
#define MAXWASTEONTILE 7 //how much waste can be on a tile
#define CAMEOPER25BUSH 1 //cameo per 25 bush. Last few not counted. 127 is the same as 120, namely 4. 24 gets 0.
#define CAMEOFORTREE 2   //cameo for a tree being there
#define CAMEOFORROAD -2 //cameo lost for being on a road of any quality. Note: This does not increase a units line of sight, but merely counteracts cameo that the unit on the road may have. If the unit has none, units looking at it won't be able to see it from further than usual.
#define SPEAKDIST 15 //how far away a unit can be to talk to another unit (communication wrapper allows info exchange/requests)
#define ANIMALSIZETOFOODAMOUNT 40
//function declarations
int geneMixer(int p1, int p2);

//variable declarations
extern vector<vector<tile> > map;
extern allunits allUnits;
extern unsigned long long frames;
extern vector<disease> allDiseases;
extern vector<vector<vector<vector<metabool> > > > mapseenhive; //[player][hive][y][x]
extern allminds allMinds;
extern vector<creatureChangeLog> allCreatureChanges;
//extern vector<animalChangeLog> allAnimalChanges;
extern ofstream *unitChangeFile;
extern ofstream *animalChangeFile;
extern curLoopVar curLoops;
extern vector<objectDescriptor> allObjectDesc;
extern bool tileConstructorAllowed;
extern vector<animal*> allAnimals;
extern vector<species> allSpecies;

#endif	/* GLOBALS_H */
