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
    bool b;
public:
    metabool();
    bool get();
    friend void unit::seeunit();
    friend void unit::seehive(int);
    friend void unit::unseehive(int);
    friend void unit::unseeunit();
};

#endif	/* METABOOL_H */

