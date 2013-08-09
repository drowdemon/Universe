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

#endif	/* DATASTRUCTURES_H */

