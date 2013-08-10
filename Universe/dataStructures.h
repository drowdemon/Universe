#ifndef DATASTRUCTURES_H
#define	DATASTRUCTURES_H

class diseaseInfo
{
public:
    int disease;
    int time;
    int multiplier; //This is how much to multiply the costs of the disease by.
    bool flipDir; //started decreasing multiplier. true=yes. 
    diseaseInfo(int d, int t=0, int m=1);
    operator int();
};

class point
{
public:
    int x;
    int y;
    point(int px, int py);
};

class unitChangeLog
{
private:
    int xorig; //id
    int yorig; //id
    int player;
    int index;
    int xmod;
    int ymod;
    int healthmod;
    int energymod;
    int hungermod;
    int sleepmod;
    int pregnantmod;
public:
    unitChangeLog(int xo, int yo, int p, int i, int xm, int ym, int hm, int em, int hunm, int sm, int pm);
    static void update(int xo, int yo, int p, int i, int xm, int ym, int hm, int em, int hunm, int sm, int pm); //comment the body of this for no gui.
    static void communicate();
};

#endif	/* DATASTRUCTURES_H */

