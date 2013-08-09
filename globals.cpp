#include "tile.h"
#include "unit.h"
#include "disease.h"
#include "metabool.h"
#include "hivemind.h"
#include <vector>

using namespace std;

vector<vector<tile> > map; 
vector<vector<unit> > allUnits; //for each player, all of the units
unsigned long long frames=0;
vector<disease> allDiseases;
vector<vector<vector<metabool> > > mapseenunit; //[player][y][x]
vector<vector<vector<metabool> > > mapseenhive; //[player][y][x]
vector<vector<hiveMind> > allMinds;