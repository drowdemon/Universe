#include "throw.h"
#include "object.h"
#include "unit.h"
#include <vector>
#include <cstdlib>
#include "globals.h"

using namespace std;

const int Throwing::StrengthToSpeed=20;      //These are so arbitrary its not even funny. For all I know, objects will fly across the map in milliseconds or refuse to move at all. Especially since I don't know the scale of what things will weigh or how strong creatures will be. Will average by 5 or 5000.
const int Throwing::BaseSpeed=500;
const int Throwing::WeightSpeedSubt=30;
const int Throwing::xpToSpeed=2;
const int Throwing::HeightDiffSpeedChange=-2; //if height increases, speed should decrease, so this is negative
const int Throwing::SpeedDecPerWeight=5;
const int Throwing::xpDiffToLearn=500;
const int Throwing::maxLearnDist=5;

Throwing::Throwing(int e)
{
    experience=e;
}

void Throwing::Throw(int objIndex, unit* who, int x, int y)
{
    int objY=who->carrying[objIndex].y;
    int objX=who->carrying[objIndex].x;
    map[objY][objX].allObjects.push_back(who->carrying[objIndex]);
    object *what=&(map[objY][objX].allObjects[map[objY][objX].allObjects.size()-1]);
    what->x=who->x;
    what->y=who->y;
    who->carrying.erase(who->carrying.begin()+objIndex);
    what->heldByIndex=what->heldByPlayer=-1;
    what->index=map[objY][objX].allObjects.size()-1;
    what->speed=Throwing::BaseSpeed+(Throwing::StrengthToSpeed*who->strength/100)-(Throwing::WeightSpeedSubt*what->weight/100)+(Throwing::xpToSpeed*experience/100);
    int accX=rand()%1000;
    int accY=rand()%1000;
    int sum=0;
    for(unsigned int i=0; i<accuracyX.size(); i++)
    {
        sum+=accuracyX[i];
        if(accX<sum)
        {
            what->toX=x+(i-((accuracyX.size()-1)/2));
            if(what->toX<0)
                what->toX=0;
            else if(what->toX>=MAPSIZE)
                what->toX=MAPSIZE-1;
            break;
        }
    }
    for(unsigned int i=0; i<accuracyY.size(); i++)
    {
        sum+=accuracyY[i];
        if(accY<sum)
        {
            what->toY=y+(i-((accuracyY.size()-1)/2));
            if(what->toY<0)
                what->toY=0;
            else if(what->toY>=MAPSIZE)
                what->toY=MAPSIZE-1;
            break;
        }
    }
}