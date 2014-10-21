#include "food.h"
#include <cstdlib>
#include "disease.h"
#include <vector>
#include "globals.h"

using namespace std;

food::food(short twg, short rs, unsigned short rr, short n, short gtv, unsigned short origN, bool veg)
{
    timeWhileGood=twg;
    rotSeverity=rs;
    rotRate=rr;
    nutrition=n;
    goodTimeVariation=gtv;
    timeWhileGood += (rand()%(goodTimeVariation*2+1))-goodTimeVariation;
    origNutrition=origN;
    vegetarian=veg;
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
        if(frames%rotRate==0)
        {
        	rotSeverity++;
        	if(rotSeverity%2==0)
        		nutrition--;
        }
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

bool food::getVegetarian()
{
	return vegetarian;
}
