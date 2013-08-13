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
    for(int i=0; i<NUMPLAYERS; i++)
    {
        mapseenunit[i].resize(MAPSIZE);
        mapseenhive[i].resize(MAPSIZE);
        for(int j=0; j<MAPSIZE; j++)
        {
            mapseenunit[i][j].resize(MAPSIZE);
            mapseenhive[i][j].resize(MAPSIZE);
        }
    }
    for(unsigned int i=0; i<map.size(); i++)
    {
        map[i].resize(MAPSIZE);
        for(unsigned int j=0; j<map[i].size(); j++)
        {
            map[i][j].x=j;
            map[i][j].y=i;
        }
    }
    unitChangeFile = new ofstream("unitChanges");
    
    allObjectDesc.push_back(objectDescriptor(3,1,OBJECT_SMALLWOOD,true,false,false,0,-1,food()));
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
            outf << "<" << (int)map[i][j].road << "," << (int)map[i][j].water << "," << (int)map[i][j].waste << "," << (int)map[i][j].animal << "," << (int)map[i][j].bush << "," << (int)map[i][j].tree << "," << (int)map[i][j].height << "," << ((map[i][j].uniton) ? 1 : 0) << ">";
        }
        outf << endl;
    }
}

int geneMixer(int p1, int p2)
{
    int singleunit=1;
    int extra=0;
    int sum=p1+p2;
    int ret=0;
    if(sum>100)
    {
        singleunit=sum/100;
        extra=sum%100;
    }
    for(int i=0; i<(sum-extra)/singleunit; i++)
        ret+=(rand()%2)*singleunit;
    for(int i=0; i<extra; i++)
        ret+=(rand()%2);
    if(ret==0)
        return (p1+p2)/2;
    return ret;
}

int main()
{
    srand(time(NULL));
    init();
    
    allUnits.data[0].push_back(unit(0,0,10,false,10,20,10,10,30,5,175,50,2,10,15,10,13));
    unitChangeLog::update(10,10,0,0,0,0,allUnits.data[0][0].health,allUnits.data[0][0].energy,allUnits.data[0][0].hunger,allUnits.data[0][0].sleep,allUnits.data[0][0].pregnant);
    //allUnits.data[0][0].moveToX=20;
    map[10][10].uniton=true;
    map[10][10].unitplayer=0;
    map[10][10].unitindex=0;
    allUnits.data[0].push_back(unit(0,1,10,true,10,20,10,11,30,5,175,50,2,10,15,10,13));
    unitChangeLog::update(allUnits.data[0][1].x,allUnits.data[0][1].y,0,1,0,0,allUnits.data[0][1].health,allUnits.data[0][1].energy,allUnits.data[0][1].hunger,allUnits.data[0][1].sleep,allUnits.data[0][1].pregnant);
    //allUnits.data[0][0].moveToX=20;
    map[11][10].uniton=true;
    map[11][10].unitplayer=0;
    map[11][10].unitindex=0;
    
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
                for(unsigned int k=0; k<map[i][j].allObjects.size(); k++)
                {
                    if(!map[i][j].allObjects[k].rot())
                    {
                        map[i][j].allObjects.erase(map[i][j].allObjects.begin()+k);
                        k--;
                    }
                }
            }
        }
        for(unsigned int i=0; i<allMinds.data.size(); i++)
        {
            curLoops.hivePlayer=i;
            for(unsigned int j=0; j<allMinds.data[i].size(); j++)
            {
                curLoops.hiveIndex=j;
                allMinds.data[i][j].act();
            }
        }
        for(unsigned int i=0; i<allUnits.data.size(); i++)
        {
            curLoops.unitPlayer=i;
            for(unsigned int j=0; j<allUnits.data[i].size(); j++)
            {
                curLoops.unitIndex=j;
                for(unsigned int k=0; k<allUnits.data[i][j].carrying.size(); k++)
                {
                    if(!allUnits.data[i][j].carrying[k].rot())
                    {
                        allUnits.data[i][j].carrying.erase(allUnits.data[i][j].carrying.begin()+k);
                        k--;
                    }
                }
                if(!allUnits.data[i][j].nextFrame())
                {
                    allUnits.data[i][j].die();
                    j--;
                }
            }
        }
        unitChangeLog::communicate(); //comment for no gui
    }
    return 0;
}