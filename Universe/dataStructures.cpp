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

creatureChangeLog::creatureChangeLog(int xo, int yo, int p, int i, int xm, int ym, int hm, int em, int hunm, int sm, int pm, vector<point> *s)
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
void creatureChangeLog::update(int xo, int yo, int p, int i, int xm, int ym, int hm, int em, int hunm, int sm, int pm, vector<point> *s)
{
    allCreatureChanges.push_back(creatureChangeLog(xo, yo, p, i, xm, ym, hm, em, hunm, sm, pm, s));
}
void creatureChangeLog::communicate()
{
    for(unsigned int i=0; i<allCreatureChanges.size(); i++)
    {
        *unitChangeFile << "<" << allCreatureChanges[i].xorig << "," << allCreatureChanges[i].yorig << "," << allCreatureChanges[i].xmod << "," << allCreatureChanges[i].ymod << "," << allCreatureChanges[i].player << "," << allCreatureChanges[i].index << "," << allCreatureChanges[i].energymod << "," << allCreatureChanges[i].hungermod << "," << allCreatureChanges[i].sleepmod << "," << allCreatureChanges[i].healthmod << "," << allCreatureChanges[i].pregnantmod << ">";
        if(allCreatureChanges[i].sight==NULL)
            *unitChangeFile << "<-1>" << endl;
        else if(allCreatureChanges[i].sight->size()==0)
            *unitChangeFile << "<-2>" << endl;
        else
        {
            *unitChangeFile << "<";
            for(unsigned int j=0; j<allCreatureChanges[i].sight->size(); j++)
            {
                *unitChangeFile << ((*allCreatureChanges[i].sight)[j]).x << "," << ((*allCreatureChanges[i].sight)[j]).y << ",";
            }
            *unitChangeFile << "-3>" << endl;
        }
    }
    *unitChangeFile << endl; //blank line signifies frame
    for(unsigned int i=0; i<allCreatureChanges.size(); i++)
        delete allCreatureChanges[i].sight;
    allCreatureChanges.clear();
}

/*animalChangeLog::animalChangeLog(int xo, int yo, int i, int xm, int ym, int hm, int hunm, int sm)
{
    xorig=xo;
    yorig=yo;
    index=i;
    xmod=xm;
    ymod=ym;
    healthmod=hm;
    hungermod=hunm;
    sleepmod=sm;
}
void animalChangeLog::update(int xo, int yo, int i, int xm, int ym, int hm, int hunm, int sm)
{
    allAnimalChanges.push_back(animalChangeLog(xo, yo, i, xm, ym, hm, hunm, sm));
}
void animalChangeLog::communicate()
{
    for(unsigned int i=0; i<allAnimalChanges.size(); i++)
    {
        *animalChangeFile << "<" << allAnimalChanges[i].xorig << "," << allAnimalChanges[i].yorig << "," << allAnimalChanges[i].xmod << "," << allAnimalChanges[i].ymod << "," << allAnimalChanges[i].index << "," << allAnimalChanges[i].hungermod << "," << allAnimalChanges[i].sleepmod << "," << allAnimalChanges[i].healthmod << ">" << endl;
    }
    *animalChangeFile << endl; //blank line signifies frame
    allAnimalChanges.clear();
}*/

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
