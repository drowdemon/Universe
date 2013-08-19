#include "throw.h"
#include "object.h"
#include "unit.h"
#include <vector>
#include <cstdlib>
#include "globals.h"
#include "currLoopVar.h"
#include <cmath>

using namespace std;

const int Throwing::StrengthToSpeed=20;      //These are so arbitrary its not even funny. For all I know, objects will fly across the map in milliseconds or refuse to move at all. Especially since I don't know the scale of what things will weigh or how strong creatures will be. Will average by 5 or 5000.
const int Throwing::BaseSpeed=500;
const int Throwing::WeightSpeedSubt=30;
const int Throwing::xpToSpeed=2;
const int Throwing::HeightDiffSpeedChange=-2; //if height increases, speed should decrease, so this is negative
const int Throwing::SpeedDecPerWeight=5;
const int Throwing::xpDiffToLearn=500;
const int Throwing::maxLearnDistSquared=25;
const int Throwing::xpToAccuracy=5;
const int Throwing::seenToXp=2;
const int Throwing::MovingAccuracyReduction=100;

Throwing::Throwing(int e)
{
    experience=e;
    accStageX=accStageY=0;
    //These are all pretty random. And they probably should vary more.
    accuracyX.resize(11);
    accuracyX[0]=30;
    accuracyX[10]=30;//60
    accuracyX[1]=90;
    accuracyX[9]=90;//240
    accuracyX[2]=120;
    accuracyX[8]=120;//480
    accuracyX[3]=110;
    accuracyX[7]=110;//700
    accuracyX[4]=100;
    accuracyX[6]=100;//900
    accuracyX[5]=100;//1000
    
    accuracyY.resize(11);
    accuracyY[0]=30;
    accuracyY[10]=30;//60
    accuracyY[1]=90;
    accuracyY[9]=90;//240
    accuracyY[2]=120;
    accuracyY[8]=120;//480
    accuracyY[3]=110;
    accuracyY[7]=110;//700
    accuracyY[4]=100;
    accuracyY[6]=100;//900
    accuracyY[5]=100;//1000
}

void Throwing::Throw(int objIndex, unit* who, int x, int y, bool moving)
{
    int objY=who->y;
    int objX=who->x;
    map[objY][objX].allObjects.push_back(who->carrying[objIndex]);
    object *what=map[objY][objX].allObjects[map[objY][objX].allObjects.size()-1];
    what->x=who->x;
    what->y=who->y;
    who->carrying.erase(who->carrying.begin()+objIndex);
    what->heldByIndex=what->heldByPlayer=-1;
    what->index=map[objY][objX].allObjects.size()-1;
    what->speed=Throwing::BaseSpeed+(Throwing::StrengthToSpeed*who->strength/100)-(Throwing::WeightSpeedSubt*what->weight/100)+(Throwing::xpToSpeed*experience/100);
    int accX=rand()%1000;
    int accY=rand()%1000;
    int sum=0;
    vector<int> tempaccX=accuracyX;
    vector<int> tempaccY=accuracyY;
    if(moving)
        tempUnLearn(tempaccX, tempaccY, Throwing::MovingAccuracyReduction);
    for(unsigned int i=0; i<tempaccX.size(); i++)
    {
        sum+=accuracyX[i];
        if(accX<sum)
        {
            what->toX=x+((signed int)i-(signed int)((tempaccX.size()-1)/2));
            if(what->toX<0)
                what->toX=0;
            else if(what->toX>=MAPSIZE)
                what->toX=MAPSIZE-1;
            break;
        }
    }
    sum=0;
    for(unsigned int i=0; i<tempaccY.size(); i++)
    {
        sum+=tempaccY[i];
        if(accY<sum)
        {
            what->toY=y+((signed int)i-(signed int)((tempaccY.size()-1)/2));
            if(what->toY<0)
                what->toY=0;
            else if(what->toY>=MAPSIZE)
                what->toY=MAPSIZE-1;
            break;
        }
    }
    learn();
}

void Throwing::learn()
{
    experience++;
    int xSize,ySize;
    if((xSize=accuracyX.size())==1 && (ySize=accuracyY.size())==1)
        return;
    xSize--;
    ySize--;
    if(experience%Throwing::xpToAccuracy==0)
    {
        if(xSize>0)
        {
            if(accuracyX[0]>0)
            {
                accuracyX[0]--;
                accuracyX[accStageX+1]++;
            }
            if(accuracyX[xSize]>0)
            {
                accuracyX[xSize]--;
                accuracyX[xSize-1-accStageX]++;
            }
            if(accStageX==(xSize-2)/2) //middle
                accStageX=0;
            else
                accStageX++;
            if(accuracyX[0]==0 && accuracyX[xSize]==0)
            {
                accuracyX.pop_back();
                accuracyX.erase(accuracyX.begin());
            }
        }
        if(ySize>0)
        {
            if(accuracyY[0]>0)
            {
                accuracyY[0]--;
                accuracyY[accStageY+1]++;
            }
            if(accuracyY[ySize]>0)
            {
                accuracyY[ySize]--;
                accuracyY[ySize-1-accStageY]++;
            }
            if(accStageY==(ySize-2)/2) //middle
                accStageY=0;
            else
                accStageY++;
            if(accuracyY[0]==0 && accuracyY[ySize]==0)
            {
                accuracyY.pop_back();
                accuracyY.erase(accuracyY.begin());
            }
        }
    }
}

void Throwing::learn(unit* student, unit* teacher)
{
    if(student->player!=curLoops.unitPlayer || student->index!=curLoops.unitIndex)
        return;
    if(pow(student->x-teacher->x,2)+pow(student->y-teacher->y,2)+pow(map[student->y][student->x].height-map[teacher->y][teacher->x].height,2)<Throwing::maxLearnDistSquared) //in range
    {
        if(teacher->throwSkill.threw)
        {
            numSeen++;
            if(numSeen==Throwing::seenToXp)
            {
                numSeen=0;
                learn();
            }
        }
    }
}

void Throwing::tempUnLearn(vector<int>& accX, vector<int>& accY, unsigned int qty)
{
//  accX=accuracyX;
//  accY=accuracyY;
    int tempqty=qty;
    int tempstageX=accStageX;
    int tempstageY=accStageY;
    while(tempqty>0)
    {
        accX[0]++;
        accX[1+tempstageX]--;
        accX[accX.size()-1]++;
        accX[accX.size()-2-tempstageX]--;
        if(tempstageX==0)
        {
            accX.insert(accX.begin(),0);
            accX.push_back(0);
            tempstageX=(accX.size()-3)/2;
        }
        
        accY[0]++;
        accY[1+tempstageY]--;
        accY[accY.size()-1]++;
        accY[accY.size()-2-tempstageY]--;
        if(tempstageY==0)
        {
            accY.insert(accY.begin(),0);
            accY.push_back(0);
            tempstageY=(accY.size()-3)/2;
        }
    }
}