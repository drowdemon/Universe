#include "food.h"
#include <cstdlib>
#include "disease.h"
#include <vector>
#include "globals.h"

using namespace std;

food::food(short twg, short rs, short n, short gtv, short origN)
{
    timeWhileGood=twg;
    rotSeverity=rs;
    nutrition=n;
    goodTimeVariation=gtv;
    timeWhileGood+=((rand()%(gtv*2+1))-gtv);
    origNutrition=origN;
}

vector<int> food::rot()
{
    if(timeWhileGood>0)
    {
        timeWhileGood--;
        return vector<int>();
    }
    else
    {
        vector<int> ret;
        rotSeverity++;
        if(rotSeverity%2==0)
            nutrition--;
        if(rotSeverity==origNutrition*6) //at *2 nutrition==0
        {
            ret.push_back(-1);
            return ret;
        }
        for(unsigned int i=0; i<allDiseases.size(); i++)
        {
            if((allDiseases[i].transmit&ROTTEN_FOOD_TRANSMIT)>0)
            {
                if(rotSeverity>(rand()%(origNutrition)))
                    ret.push_back(i);
            }
        }
        return ret;
    }
}

bool food::operator!()
{
    if(timeWhileGood==-1 && rotSeverity==-1 && nutrition==-1 && goodTimeVariation==-1 && origNutrition==-1)
        return true;
    return false;
}