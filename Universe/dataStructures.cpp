#include <vector>

#include "dataStructures.h"
#include "globals.h"

diseaseInfo::diseaseInfo(int d, int t, int m)
{
    disease=d;
    time=t;
    multiplier=m;
    flipDir=false;
}
diseaseInfo::operator  int()
{
    return disease;
}

point::point(int px, int py)
{
    x=px;
    y=py;
}

unitChangeLog::unitChangeLog(int xo, int yo, int p, int i, int xm, int ym, int hm, int em, int hunm, int sm, int pm)
{
    xorig=xo;
    yorig=yo;
    player=p;
    index=i;
    xmod=xm;
    ymod=ym;
    healthmod=hm;
    energymod=em;
    hungermod=hunm;
    sleepmod=sm;
    pregnantmod=pm;
}
void unitChangeLog::update(int xo, int yo, int p, int i, int xm, int ym, int hm, int em, int hunm, int sm, int pm)
{
    unitChangeLog log(xo, yo, p, i, xm, ym, hm, em, hunm, sm, pm);
    allUnitChanges.push_back(log);
}
void unitChangeLog::communicate()
{
    for(unsigned int i=0; i<allUnitChanges.size(); i++)
    {
        *unitChangeFile << "<" << allUnitChanges[i].xorig << "," << allUnitChanges[i].yorig << "," << allUnitChanges[i].xmod << "," << allUnitChanges[i].ymod << "," << allUnitChanges[i].player << "," << allUnitChanges[i].index << "," << allUnitChanges[i].energymod << "," << allUnitChanges[i].hungermod << "," << allUnitChanges[i].sleepmod << "," << allUnitChanges[i].healthmod << "," << allUnitChanges[i].pregnantmod << ">" << endl;
    }
    allUnitChanges.clear();
}