#include "tile.h"
#include "unit.h"
#include "globals.h"
#include "metabool.h"
#include <cmath>
#include <cstdlib>
#include "hivemind.h"
#include "allunits.h"

tile::tile(unsigned char r, unsigned short w, short h, unsigned char wst, bool uo, unsigned short a, unsigned char sw, short px, short py, short up, short ui, unsigned char b, unsigned char t)
{
    road=r;
    water=w;
    height=h;
    waste=wst;
    uniton=uo;
    animal=a;
    smallWood=sw;
    x=px;
    y=py;
    unitindex=ui;
    unitplayer=up;    
    wasteMoved=false;
    bush=b;
    tree=t;
}
bool tile::walkable(short origHeight, short fx, short fy)
{
    if(water>2) //not very shallow water
        return false; //nope
    if(animal!=0) //animal on that tile
        return false; //nope
    if(uniton) //there is already a unit there, assuming all units are 1x1 tiles
        return false;
    if(tree>0)
        return false; //cannot walk on trees
    if(origHeight>height+1) //you can only tolerate a height difference of 1. Maybe this should be increased if height is to be more gradually changing. Also, you can't climb up things easily, but you can fall.
        return false;
   // if(origHeight<height-1) //ditto in the other direction
   //   return false;
    if(abs(fx-x)>1 || abs(fy-y)>1) //if this tile is more than 1 away vertically or horizontally
        return false; 
    return true;
}
void tile::moveWater(int tx, int ty)
{
    if(water-height>map[ty][tx].water+1-map[ty][tx].height) //1 tolerance, no decimals. Accounts for height of underlying land
    {
        water--;
        map[ty][tx].water++; //let water flow downhill
        if(waste>0 && !wasteMoved) //move waste too!
        {
            waste--;
            map[ty][tx].waste++;
            map[ty][tx].wasteMoved=true;
        }
        if(disease.size()>0) //let any disease flow with it. Bwa ha ha!
        {
            for(unsigned int i=0; i<disease.size(); i++)
            {
                bool good=true;
                for(unsigned int j=0; j<map[ty][tx].disease.size(); j++)
                {
                    if(map[ty][tx].disease[j]==disease[i])
                    {
                        good=false;
                        break;
                    }
                }
                if(good)
                {
                    if(rand()%10000<allDiseases[disease[i]].waterSpreadability)
                    {
                        map[ty][tx].disease.push_back(disease[i]);
                        map[ty][tx].diseaseTime.push_back(diseaseTime[i]);
                    }
                }
            }
        }
    }
    if(water-height<map[ty][tx].water-1-map[ty][tx].height)
    {
        water++;
        map[ty][tx].water--;
        if(map[ty][tx].waste>0 && !map[ty][tx].wasteMoved) //move waste
        {
            map[ty][tx].waste--;
            waste++;
            wasteMoved=true;
        }
        if(map[ty][tx].disease.size()>0) //let any disease flow with it. Bwa ha ha!
        {
            for(unsigned int i=0; i<map[ty][tx].disease.size(); i++)
            {
                bool good=true;
                for(unsigned int j=0; j<disease.size(); j++)
                {
                    if(map[ty][tx].disease[i]==disease[j])
                    {
                        good=false;
                        break;
                    }
                }
                if(good)
                {
                    if(rand()%10000<allDiseases[map[ty][tx].disease[i]].waterSpreadability)
                    {
                        disease.push_back(map[ty][tx].disease[i]);
                        diseaseTime.push_back(map[ty][tx].diseaseTime[i]);
                    }
                }
            }
        }
    }
}
void tile::spreadDisease()
{
    if(frames%INFECTRATE==0)
    {
        for(unsigned int h=0; h<disease.size(); h++)
        {
            if(diseaseTime[h]>=allDiseases[disease[h]].duration)
            {
                diseaseTime.erase(diseaseTime.begin()+h); //no more disease
                disease.erase(disease.begin()+h);
                h--;
            }
            diseaseTime[h]++;
            for(int i = (y>allDiseases[disease[h]].spreadabilityArea) ? (y-allDiseases[disease[h]].spreadabilityArea) : 0; i < (MAPSIZE-y<allDiseases[disease[h]].spreadabilityArea) ? (y+allDiseases[disease[h]].spreadabilityArea) : MAPSIZE; i++)
            {
                for(int j = (x>allDiseases[disease[h]].spreadabilityArea) ? (x-allDiseases[disease[h]].spreadabilityArea) : 0; j < (MAPSIZE-x<allDiseases[disease[h]].spreadabilityArea) ? (x+allDiseases[disease[h]].spreadabilityArea) : MAPSIZE; j++)
                {
                    if(map[i][j].uniton)
                    {
                        int immunityloss=0;
                        for(unsigned int d=0; d<allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased.size(); d++)
                            immunityloss+=allDiseases[allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased[d]].immunCost;
                        if(rand()%10000<allDiseases[disease[h]].spreadabilityChance-((allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].immunity-immunityloss>0)?(allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].immunity-immunityloss):0)+((MAXHEALTH-allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].health)*allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].healthDiseaseInc))
                        {
                            bool good=true;
                            for(unsigned int d=0; d<allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased.size(); d++)
                            {
                                if(disease[h]==allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased[d].disease)
                                {
                                    good=false;
                                    allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased[d].multiplier++;
                                    allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased[d].flipDir=false;
                                    break;
                                }
                            }
                            if(good)
                            {
                                allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].diseased.push_back(disease[h]);
                                allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].strength-=allDiseases[disease[h]].permStrCost;
                                allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].intelligence-=allDiseases[disease[h]].permIntelCost;
                                allUnits.data[map[i][j].unitplayer][map[i][j].unitindex].immunity-=allDiseases[disease[h]].permImmunCost;
                            }
                        }
                    }
                }
            }
        }
    }
}

tile* tile::get(unit& u)
{
    if(mapseenunit[u.player][y][x].get())
        return this;
    return NULL;
}
tile* tile::get(hiveMind& h)
{
    if(mapseenhive[h.player][h.index][y][x].get())
        return this;
    return NULL;
}