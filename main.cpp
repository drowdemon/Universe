#include <cstdlib>
#include <vector>
#include "tile.h"
#include "unit.h"
#include "globals.h"
using namespace std;

void init()
{
    map.resize(200); //randomly chosen by me
    for(int i=0; i<map.size(); i++)
    {
        map[i].resize(200);
        for(int j=0; j<map[i].size(); j++)
        {
            map[i][j].x=j;
            map[i][j].y=i;
        }
    }
    allUnits.resize(2);
}

int main()
{
    while(true) //it never closes. Somewhat inconvenient. But its not bothering to use win32 or glut or mfc, with good reason, so this is the best I could do. 
    {
        for(int i=0; i<allUnits.size(); i++)
        {
            for(int j=0; j<allUnits[i].size(); j++)
            {
                allUnits[i][j].nextFrame();
            }
        }
        frames++;
    }
    return 0;
}

