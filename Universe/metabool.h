/* 
 * File:   metabool.h
 * Author: marc
 *
 * Created on August 9, 2013, 10:21 AM
 */

#ifndef METABOOL_H
#define	METABOOL_H

#include "unit.h"

class metabool
{
private:
    unsigned char b; //0=false, 1=true, 2=true for landscape but not units
public:
    metabool();
    unsigned char get();
    friend void unit::seeunit();
    friend void unit::seehive(int);
    friend void unit::unseehive(int);
    friend void unit::unseeunit();
};

#endif	/* METABOOL_H */

