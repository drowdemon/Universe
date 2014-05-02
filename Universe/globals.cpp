#include "tile.h"
#include "unit.h"
#include "disease.h"
#include "metabool.h"
#include "hivemind.h"
#include <vector>
#include "dataStructures.h"
#include "fstream"
#include "allunits.h"
#include "allminds.h"
#include "currLoopVar.h"
#include "object.h"
#include "Animalia/animal.h"
#include "Animalia/species.h"

using namespace std;

vector<vector<tile> > map; 
//vector<vector<unit> > allUnits; //for each player, all of the units
allunits allUnits;
unsigned long long frames=0;
vector<disease> allDiseases;
vector<vector<vector<metabool> > > mapseenunit; //[player][y][x]
vector<vector<vector<metabool> > > mapseenhive; //[player][y][x]
allminds allMinds;
vector<unitChangeLog> allUnitChanges;
vector<animalChangeLog> allAnimalChanges;
ofstream *unitChangeFile;
ofstream *animalChangeFile;
curLoopVar curLoops;
vector<objectDescriptor> allObjectDesc;
bool tileConstructorAllowed=true;
vector<animal*> allAnimals;
vector<species> allSpecies;