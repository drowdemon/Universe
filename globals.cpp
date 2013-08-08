#include "tile.h"
#include "unit.h"
#include <vector>

using namespace std;

vector<vector<tile> > map; 
vector<vector<unit> > allUnits; //for each player, all of the units
unsigned long long frames=0;