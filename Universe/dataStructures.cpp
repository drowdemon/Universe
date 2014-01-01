#include <vector>

#include "dataStructures.h"
#include "globals.h"
#include "tile.h"
#include "object.h"

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

pointRad::pointRad(int px, int py, int pr) : point(px, py)
{
    rad=pr;
}

visionObstacle::visionObstacle(int dx, int dy) : point(dx,dy)
{
    if((dx>0 && dy>0) || (dx<0 && dy<0)) //q I and q III : q I s2<1; q III s1<s2
    {
        slope1=((double)dy-0.5) / ((double)dx+0.5); 
        slope2=((double)dy+0.5) / ((double)dx-0.5);
    }
    else if((dx<0 && dy>0) || (dx>0 && dy<0)) //q II and q IV : q II s2<1; q IV s1<s2
    {
        slope1=((double)dy+0.5) / ((double)dx+0.5);
        slope2=((double)dy-0.5) / ((double)dx-0.5);
    }
    else if(dy==0 && dx>0) //on +x axis
    {
        slope1=((double)dy+0.5) / ((double)dx-0.5); //s2<s1
        slope2=((double)dy-0.5) / ((double)dx-0.5);
    }
    else if(dy==0 && dx<0) //on -x axis
    {
        slope1=((double)dy-0.5) / ((double)dx+0.5); //s2<s1
        slope2=((double)dy+0.5) / ((double)dx+0.5);
    }
    else if(dx==0 && dy>0) //on +y axis
    {
        slope1=((double)dy-0.5) / ((double)dx+0.5); //s2<s1
        slope2=((double)dy-0.5) / ((double)dx-0.5); 
    }
    else if(dx==0 && dy<0) //on -y axis
    {
        slope1=((double)dy+0.5) / ((double)dx-0.5); //s2<s1
        slope2=((double)dy+0.5) / ((double)dx+0.5); 
    }
}

unitChangeLog::unitChangeLog(int xo, int yo, int p, int i, int xm, int ym, int hm, int em, int hunm, int sm, int pm, vector<point> *s)
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
    if(s==NULL)
        sight=NULL;
    else
    {
        sight = new vector<point>;
        for(unsigned int i=0; i<s->size(); i++)
            sight->push_back(point(((*s)[i]).x, ((*s)[i]).y));
    }
}
void unitChangeLog::update(int xo, int yo, int p, int i, int xm, int ym, int hm, int em, int hunm, int sm, int pm, vector<point> *s)
{
    allUnitChanges.push_back(unitChangeLog(xo, yo, p, i, xm, ym, hm, em, hunm, sm, pm, s));
}
void unitChangeLog::communicate()
{
    for(unsigned int i=0; i<allUnitChanges.size(); i++)
    {
        *unitChangeFile << "<" << allUnitChanges[i].xorig << "," << allUnitChanges[i].yorig << "," << allUnitChanges[i].xmod << "," << allUnitChanges[i].ymod << "," << allUnitChanges[i].player << "," << allUnitChanges[i].index << "," << allUnitChanges[i].energymod << "," << allUnitChanges[i].hungermod << "," << allUnitChanges[i].sleepmod << "," << allUnitChanges[i].healthmod << "," << allUnitChanges[i].pregnantmod << ">";
        if(allUnitChanges[i].sight==NULL)
            *unitChangeFile << "<-1>" << endl;
        else if(allUnitChanges[i].sight->size()==0)
            *unitChangeFile << "<-2>" << endl;
        else
        {
            *unitChangeFile << "<";
            for(unsigned int j=0; j<allUnitChanges[i].sight->size(); j++)
            {
                *unitChangeFile << ((*allUnitChanges[i].sight)[j]).x << "," << ((*allUnitChanges[i].sight)[j]).y << ",";
            }
            *unitChangeFile << "-3>" << endl;
        }
    }
    *unitChangeFile << endl; //blank line signifies frame
    for(unsigned int i=0; i<allUnitChanges.size(); i++)
        delete allUnitChanges[i].sight;
    allUnitChanges.clear();
}

/*pubTile::pubTile(vector<short> d, vector<short> dt, vector<object> ao, unsigned char r, unsigned short w, short h, unsigned char wst, bool uo, unsigned short a, *unsigned char sw,/ short px, short py, short up, short ui, unsigned char b, unsigned char t)
{
    disease=d;
    diseaseTime=dt;
    allObjects=ao;
    road=r;
    water=w;
    height=h;
    waste=wst;
    uniton=uo;
    animal=a;
    //smallWood=sw;
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
    for(unsigned int i=0; i<t->allObjects.size(); i++)
        allObjects.push_back(*(t->allObjects[i]));
    road=t->road;
    water=t->water;
    height=t->height;
    waste=t->waste;
    uniton=t->uniton;
    animal=t->animal;
    //smallWood=t->smallWood;
    x=t->x;
    y=t->y;
    unitindex=t->unitindex;
    unitplayer=t->unitplayer;    
    bush=t->bush;
    tree=t->tree;
}*/