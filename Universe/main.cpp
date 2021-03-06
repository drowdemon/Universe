#include <ctime>
#include <vector>
#include "tile.h"
#include "unit.h"
#include "globals.h"
#include "Animalia/animalEatingAbilities.h"
#include "mathFunction.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

void init()
{
    mapseenhive.resize(NUMPLAYERS);
    map.resize(MAPSIZE); //randomly chosen by me
    for(int i=0; i<NUMPLAYERS; i++)
    {
        mapseenhive[i].resize(MAPSIZE);
        for(int j=0; j<MAPSIZE; j++)
        {
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
    tileConstructorAllowed=false;
    unitChangeFile = new ofstream("unitChanges");
    
    allSpecies.push_back(species((short)MAXHEALTH, (short)20, (short)5, (short)-1, (short)MOVEMENTENERGY, (short)(FALLINGMULTIPLIER*100.0), (short)MAXHUNGER, (short)ENERGYCRITPOINT, (short)GESTATIONPERIOD, (short)PREGNANTENERGYCOST, (short)REPRODUCTIONENERGYCOST, (short)NEWBORNENERGY, (short)NEWBORNSLEEP, (short)NEWBORNHUNGER, (short)BIRTHHEALTHLOSS, (short)BIRTHENERGYLOSS, (short)REPRODUCTIONTIME, (short)ENERGYSOFTMAX, (short)ENERGYFROMFATPOINT, (short)ENERGYFROMFATRATE, (short)NEWBORNMINWEIGHT, (short)MOVINGSELFWEIGHTPENALTY, (short)MOVINGLIFTEDWEIGHTPENALTY, 3, TOLERABLEHEIGHTDIFF, 0, 0, NULL));
    //an animal identical to a unit
    allSpecies.push_back(species((short)MAXHEALTH, (short)20, (short)5, (short)0, (short)MOVEMENTENERGY, (short)(FALLINGMULTIPLIER*100.0), (short)MAXHUNGER, (short)ENERGYCRITPOINT, (short)GESTATIONPERIOD, (short)PREGNANTENERGYCOST, (short)REPRODUCTIONENERGYCOST, (short)NEWBORNENERGY, (short)NEWBORNSLEEP, (short)NEWBORNHUNGER, (short)BIRTHHEALTHLOSS, (short)BIRTHENERGYLOSS, (short)REPRODUCTIONTIME, (short)ENERGYSOFTMAX, (short)ENERGYFROMFATPOINT, (short)ENERGYFROMFATRATE, (short)NEWBORNMINWEIGHT, (short)MOVINGSELFWEIGHTPENALTY, (short)MOVINGLIFTEDWEIGHTPENALTY, 3, TOLERABLEHEIGHTDIFF, 10, 5, new herbivore(0, allSpecies.size(), false)));
    
    allObjectDesc.push_back(objectDescriptor(3,1,OBJECT_SMALLWOOD,true,false,false,-3,-1,food()));
    allObjectDesc.push_back(objectDescriptor(50,5,OBJECT_CORPSE,true,true,true,-2,-1,food())); //INCORRECT STATS. DEFINITELY. 
    allObjectDesc.push_back(objectDescriptor(50,5,OBJECT_GENERICANIMALCORPSE,true,true,true,-2,-1,food())); //INCORRECT STATS. DEFINITELY. 
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
            outf << "<" << (int)map[i][j].road << "," << (int)map[i][j].water << "," << (int)map[i][j].waste << "," << (int)map[i][j].animalPresent << "," << (int)map[i][j].bush << "," << (int)map[i][j].tree << "," << (int)map[i][j].height << "," << ((map[i][j].uniton) ? 1 : 0) << ">";
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

mathFunction* geneMixer(mathFunction* f1, mathFunction* f2)
{
	if(!f1 || !f2)
		return NULL;
	if(f1->terms.size()!=f2->terms.size())
	{
		cout << "Error in input to geneMixer of functions" << endl;
		return NULL;
	}
	mathFunction* ret = new mathFunction;
	for(unsigned int i=0; i<f1->terms.size(); i++)
	{
		double inner = 0;
		double outer = 0;
		double counterInner = (f1->terms[i]->innerCoef+f2->terms[i]->innerCoef)/GENEMIXERSTEPSIZE;
		double counterOuter = (f1->terms[i]->outerCoef+f2->terms[i]->outerCoef)/GENEMIXERSTEPSIZE;
		for(int i=0; i<GENEMIXERSTEPSIZE; i++)
		{
			inner+=rand()%2*counterInner;
			outer+=rand()%2*counterOuter;
		}
		ret->add(new mathTerm(outer, inner, f1->terms[i]->power, f1->terms[i]->specialFunc, geneMixer(f1->terms[i]->actOn, f2->terms[i]->actOn), geneMixer(f1->terms[i]->multBy, f2->terms[i]->multBy)));
	}
	return ret;
}

void reformat()
{
    vector<vector<int> > indexMappings;
    indexMappings.resize(allUnits.data.size());
    for(unsigned int i=0; i<allUnits.data.size(); i++)
    {
        indexMappings[i].resize(allUnits.data[i].size());
        int currDeleted=0;
        for(unsigned int j=0; j<allUnits.data[i].size(); j++)
        {
            if(!allUnits.data[i][j]) //invalid pointer
            {
                allUnits.data[i].erase(allUnits.data[i].begin()+j); //delete it
                j--;
                currDeleted++;
            }
            else //valid
            {
                allUnits.data[i][j]->index-=currDeleted;
                map[allUnits.data[i][j]->y][allUnits.data[i][j]->x].unitindex-=currDeleted;
                indexMappings[i][j]=allUnits.data[i][j]->index;
                for(unsigned int k=0; k<allUnits.data[i][j]->carrying.size(); k++)
                    allUnits.data[i][j]->carrying[k]->heldByIndex-=currDeleted;
                allUnits.data[i][j]->allCommuniques.index-=currDeleted;
            }
        }
    }
    for(unsigned int i=0; i<allUnits.data.size(); i++)
	{
		for(unsigned int j=0; j<allUnits.data[i].size(); j++)
		{
			if(allUnits.data[i][j]->fetusid!=-1)
				allUnits.data[i][j]->fetusid=indexMappings[i][allUnits.data[i][j]->fetusid];
			for(unsigned int k=0; k<NUMSKILLS; k++)
			{
				if(allUnits.data[i][j]->learningSkills[k]!=-1)
					allUnits.data[i][j]->learningSkills[k]=indexMappings[i][allUnits.data[i][j]->learningSkills[k]];
			}
		}
	}
    creatureChangeLog::update(-99999,-99999,-99999,-99999,-99999,-99999,-99999,-99999,-99999,-99999,-99999,NULL);
}

void reformatAnimals()
{
    int currDeleted=0;
    vector<int> indexMappings;
    indexMappings.resize(allAnimals.size());
    for(unsigned int i=0; i<allAnimals.size(); i++)
    {
        if(!allAnimals[i]) //invalid pointer
        {
            allAnimals.erase(allAnimals.begin()+i); //delete it
            i--;
            currDeleted++;
        }
        else //valid
        {
            allAnimals[i]->index-=currDeleted;
            map[allAnimals[i]->y][allAnimals[i]->x].animalPresent-=currDeleted;
            indexMappings[i]=allAnimals[i]->index;
			for(unsigned int k=0; k<allAnimals[i]->carrying.size(); k++)
				allAnimals[i]->carrying[k]->heldByIndex-=currDeleted;
        }
    }
    for(unsigned int i=0; i<allAnimals.size(); i++)
	{
		if(allAnimals[i]->fetusid!=-1)
			allAnimals[i]->fetusid=indexMappings[allAnimals[i]->fetusid];
	}
}

int main()
{
    srand(time(NULL));
    init();
    
    mathFunction* speed = new mathFunction();
    speed->add(new mathTerm(20, 1, 0, 0, NULL, NULL));
    
    mathFunction* strength = new mathFunction();
    strength->add(new mathTerm(10, 1, 0, 0, NULL, NULL));
    
    mathFunction* woundEnergyCost = new mathFunction();
    woundEnergyCost->add(new mathTerm(2, 1, 0, 0, NULL, NULL));
    
    mathFunction* minWeight = new mathFunction();
    minWeight->add(new mathTerm(allSpecies[0].newbornMinWeight, 1, 0, 0, NULL, NULL));
    
	mathFunction* immunity = new mathFunction();
    immunity->add(new mathTerm(175, 1, 0, 0, NULL, NULL));
		
    allUnits.data[0].push_back(new unit(false, speed, strength, 0, 5, allSpecies[0].maxHealth, 100, allSpecies[0].newbornHunger, 10, 10, allSpecies[0].newbornSleep, 20, allSpecies[0].newbornEnergy, -1, 0, woundEnergyCost, minWeight, 150, 850, 8, 30, 13, 250, 13, immunity, 200, 0, 0, 10, 50, 0));
    //allUnits.data[0][0]->minWeight=60;
    creatureChangeLog::update(10,10,0,0,0,0,allUnits.data[0][0]->health,allUnits.data[0][0]->energy,allUnits.data[0][0]->hunger,allUnits.data[0][0]->sleep,allUnits.data[0][0]->pregnant,NULL);
    //allUnits.data[0][0].moveToX=20;
    map[10][10].uniton=true;
    map[10][10].unitplayer=0;
    map[10][10].unitindex=0;
    /*allUnits.data[0].push_back(new unit(0,1,10,true,10,20,10,11,20,5,175,50,2,30,13,8,13,0,100,150,850,200));
    allUnits.data[0][1]->minWeight=60;
    unitChangeLog::update(allUnits.data[0][1]->x,allUnits.data[0][1]->y,0,1,0,0,allUnits.data[0][1]->health,allUnits.data[0][1]->energy,allUnits.data[0][1]->hunger,allUnits.data[0][1]->sleep,allUnits.data[0][1]->pregnant,NULL);
    map[11][10].uniton=true;
    map[11][10].unitplayer=0;
    map[11][10].unitindex=0;*/
    
    map[11][11].bush=100;
    
    allUnits.data[0][0]->carrying.push_back(new object(allObjectDesc[0],0,0,-1,-1,0,map[allUnits.data[0][0]->y][allUnits.data[0][0]->x].height));
        
    allAnimals.push_back(new animal(true, speed, strength, 0, 5, 100, 50, 0, 20, 20, 700, 0, 1000, -1, 1, woundEnergyCost, minWeight, 60, 50, 8, 30, 13, 250, 13, immunity, 200, 0, 0, 5));
    creatureChangeLog::update(20,20,-1,0,0,0,allAnimals[0]->health,allAnimals[0]->energy,allAnimals[0]->hunger,allAnimals[0]->sleep,allAnimals[0]->pregnant,NULL);
    map[20][20].animalPresent=0;
    
    printMap(); //comment out for no output.
    while(true) //it never closes. Somewhat inconvenient. But its not bothering to use win32 or glut or mfc, with good reason, so this is the best I could do. 
    {
        frames++;
        if(frames==0) //could ruin a lot of mod things, make everything happen all at once simply cause of overflow
        	frames++; 
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //REMOVE THIS TESTING ONLY
        if(frames==50)
        {
        	allUnits.data[0][0]->moveToX=13;
        	//allUnits.data[0][0]->speed=40;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //MAP
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for(unsigned int i=0; i<map.size(); i++)
        {
            for(unsigned int j=0; j<map[i].size(); j++)
            {
                waterFlow(i,j);
                map[i][j].spreadDisease();
                for(unsigned int k=0; k<map[i][j].allObjects.size(); k++)
                {
                    if(!map[i][j].allObjects[k]->rot())
                    {
                        map[i][j].allObjects.erase(map[i][j].allObjects.begin()+k);
                        k--;
                    }
                    else if(map[i][j].allObjects[k]->speed>0)
                        map[i][j].allObjects[k]->move();
                }
            }
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //ANIMALS
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int numAnimalsDead=0;
        for(unsigned int i=0; i<allAnimals.size(); i++)
        {
        	curLoops.animalIndex=i;
            if(!allAnimals[i])
            {
                numAnimalsDead++;
                continue;
            }
            for(unsigned int k=0; k<allAnimals[i]->carrying.size(); k++)
			{
				if(!allAnimals[i]->carrying[k]->rot())
				{
					allAnimals[i]->carrying.erase(allAnimals[i]->carrying.begin()+k);
					k--;
				}
			}
            if(!allAnimals[i]->nextFrame())
            {
                numAnimalsDead++;
            }
        }
        curLoops.animalIndex=-1;
        if(numAnimalsDead>=NUMANIMALSDEADTOREFORMAT)
            reformatAnimals();
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //HiveMinds
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for(unsigned int i=0; i<allMinds.data.size(); i++)
        {
            curLoops.hivePlayer=i;
            for(unsigned int j=0; j<allMinds.data[i].size(); j++)
            {
                curLoops.hiveIndex=j;
                allMinds.data[i][j].act();
            }
        }
        curLoops.hiveIndex=curLoops.hivePlayer=-1;
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Units
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int numDead=0;
        for(unsigned int i=0; i<allUnits.data.size(); i++)
        {
            curLoops.unitPlayer=i;
            for(unsigned int j=0; j<allUnits.data[i].size(); j++)
            {
                if(!allUnits.data[i][j])
                {
                    numDead++;
                    continue;
                }
                curLoops.unitIndex=j;
                for(unsigned int k=0; k<allUnits.data[i][j]->carrying.size(); k++)
                {
                    if(!allUnits.data[i][j]->carrying[k]->rot())
                    {
                        allUnits.data[i][j]->carrying.erase(allUnits.data[i][j]->carrying.begin()+k);
                        k--;
                    }
                }
                if(!allUnits.data[i][j]->nextFrame())
                {
                    allUnits.data[i][j]->die();
                    //j--;
                    numDead++;
                }
            }
        }
        curLoops.unitIndex=curLoops.unitPlayer=-1;
        if(numDead>=NUMDEADTOREFORMAT)
        {
            if(rand()%200==0) //information hiding
                reformat();
        }
        creatureChangeLog::communicate(); //comment for no gui
        //animalChangeLog::communicate(); //comment for no gui
    }
    return 0;
}
