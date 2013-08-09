#include <cstdlib>
#include <ctime>
#include <vector>
#include "tile.h"
#include "unit.h"
#include "globals.h"
#include <iostream>
#include <fstream>
using namespace std;

void init()
{
    mapseenunit.resize(NUMPLAYERS);
    mapseenhive.resize(NUMPLAYERS);
    map.resize(MAPSIZE); //randomly chosen by me
    mapseenunit.resize(MAPSIZE);
    mapseenhive.resize(MAPSIZE);
    for(unsigned int i=0; i<map.size(); i++)
    {
        map[i].resize(MAPSIZE);
        mapseenunit[i].resize(MAPSIZE);
        mapseenhive[i].resize(MAPSIZE);
        for(unsigned int j=0; j<map[i].size(); j++)
        {
            map[i][j].x=j;
            map[i][j].y=i;
        }
    }
    allUnits.resize(NUMPLAYERS);
    allMinds.resize(NUMPLAYERS);
}

void waterFlow(int i, int j) //i=y, j=x
{
    if(frames%WATERFLOWRATE==0)
    {
        if(i>0)
        {
            map[i][j].moveWater(i-1,j);
            if(j>0)
            {
                map[i][j].moveWater(i-1,j-1);
                map[i][j].moveWater(i,j-1);
            }
            if(j<MAPSIZE-1)
            {
                map[i][j].moveWater(i-1,j+1);
                map[i][j].moveWater(i,j+1);
            }
        }
        if(i<MAPSIZE-1)
        {
            if(j>0)
            {
                map[i][j].moveWater(i+1,j-1);
            }
            if(j<MAPSIZE-1)
            {
                map[i][j].moveWater(i+1,j+1);
            }
        }
        map[i][j].wasteMoved=false; //reset this
    }
}

void printMap()
{
    ofstream outf("map");
    if(!outf)
    {
        cout << "Error writing to map" << endl;
        exit(-1);
    }
    for(unsigned int i=0; i<map.size(); i++)
    {
        for(unsigned int j=0; j<map[i].size(); j++)
        {
            outf << "<" << (int)map[i][j].road << "," << (int)map[i][j].water << "," << (int)map[i][j].waste << "," << (int)map[i][j].animal << "," << (int)map[i][j].bush << "," << (int)map[i][j].tree << "," << (int)map[i][j].height << ">";
        }
        outf << endl;
    }
}

int main()
{
    srand(time(NULL));
    init();
    printMap(); //comment out for no output.
    
    while(true) //it never closes. Somewhat inconvenient. But its not bothering to use win32 or glut or mfc, with good reason, so this is the best I could do. 
    {
        frames++;
        for(unsigned int i=0; i<map.size(); i++)
        {
            for(unsigned int j=0; j<map[i].size(); j++)
            {
                waterFlow(i,j);
                map[i][j].spreadDisease();
            }
        }
        for(unsigned int i=0; i<allMinds.size(); i++)
        {
            for(unsigned int j=0; j<allMinds[i].size(); j++)
            {
                allMinds[i][j].act();
            }
        }
        for(unsigned int i=0; i<allUnits.size(); i++)
        {
            for(unsigned int j=0; j<allUnits[i].size(); j++)
            {
                allUnits[i][j].nextFrame();
            }
        }
    }
    return 0;
}