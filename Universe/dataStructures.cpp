#include <vector>

#include "dataStructures.h"
#include "globals.h"
#include "tile.h"

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
    *unitChangeFile << endl; //blank line signifies frame
    allUnitChanges.clear();
}

pubTile::pubTile(vector<short> d, vector<short> dt, unsigned char r, unsigned short w, short h, unsigned char wst, bool uo, unsigned short a, unsigned char sw, short px, short py, short up, short ui, unsigned char b, unsigned char t)
{
    disease=d;
    diseaseTime=dt;
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
    bush=b;
    tree=t;
}

pubTile::pubTile(tile* t)
{
    disease=t->disease;
    diseaseTime=t->diseaseTime;
    road=t->road;
    water=t->water;
    height=t->height;
    waste=t->waste;
    uniton=t->uniton;
    animal=t->animal;
    smallWood=t->smallWood;
    x=t->x;
    y=t->y;
    unitindex=t->unitindex;
    unitplayer=t->unitplayer;    
    bush=t->bush;
    tree=t->tree;
}