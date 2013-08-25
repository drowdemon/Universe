/* 
 * File:   info.h
 * Author: marc
 *
 * Created on August 24, 2013, 4:08 PM
 */

#ifndef INFO_H
#define	INFO_H

#include <tuple>

using std::tuple;

class info
{
public:
    int a;
    int b;
    int c;
    tuple<int*, int*, int*> data;
    info(int pa, int pb, int pc);
    info();
};

#endif	/* INFO_H */

