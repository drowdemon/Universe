/* 
 * File:   main.cpp
 * Author: marc
 *
 * Created on August 24, 2013, 4:08 PM
 */

#include <cstdlib>
#include "boolList.h"
#include "info.h"
#include <tuple>
#include <iostream>
#include "foo.h"
#include "boolList2.h"
#include <vector>

using namespace std;

int main() 
{
    vector<foo> vecFoo;
    vecFoo.resize(10);
    info i1(1,2,3);
    info i2;
    boolList bl;
    bl.wantedData.push_back(true);
    bl.wantedData.push_back(false);
    bl.wantedData.push_back(true);
    
    bl.conds.push_back(boolCond(2,3,-1,true,EQ)); //true
    bl.conds.push_back(boolCond(1,-1,2,false,LESS)); //true
    bl.conds.push_back(boolCond(1,-1,2,false,GREATER)); //false
    
    bl.parens.resize(2);
    bl.parens[0].push_back(0); //parens elements 1 and 2
    bl.parens[1].push_back(0); //parens elements 0,1,2. So its is ((0,1),2)
    
    bl.performOps.push_back(ANDIND); //cond0 && cond1
    bl.performOps.push_back(ORIND); //((cond0 && cond1) || cond2)
    
    i2.data=bl.run(i1.data);
    if(get<0>(i2.data))
        i2.a=*(get<0>(i2.data));
    if(get<1>(i2.data))
        i2.b=*(get<1>(i2.data));
    if(get<2>(i2.data))
        i2.c=*(get<2>(i2.data));
    cout << i2.a << " " << i2.b << " " << i2.c << endl;
    
    tuple<int*, int*, foo*> data = make_tuple(new int,new int,new foo(1,2));
    *(get<0>(data))=1;
    *(get<1>(data))=2;
    boolList2 bl2;
    bl2.wantedData.push_back(true);
    bl2.wantedData.push_back(false);
    bl2.wantedData.push_back(true);
    
    bl2.conds.push_back(boolCond(2,3,-1,true,EQ)); //
    bl2.conds.push_back(boolCond(1,-1,2,false,LESS)); //
    bl2.conds.push_back(boolCond(1,-1,2,false,GREATER)); //
    
    bl2.parens.resize(2);
    bl2.parens[0].push_back(0); //parens elements 1 and 2
    bl2.parens[1].push_back(0); //parens elements 0,1,2. So its is ((0,1),2)
    
    bl2.performOps.push_back(ANDIND); //cond0 && cond1
    bl2.performOps.push_back(ORIND); //((cond0 && cond1) || cond2)
    
    data=bl2.run(data);
    int dat1=-1;
    int dat2=-1;
    foo dat3(-1,-1);
    if(get<0>(data))
        dat1=*(get<0>(data));
    if(get<1>(data))
        dat2=*(get<1>(data));
    if(get<2>(data))
        dat3=*(get<2>(data));
    cout << dat1 << " " << dat2 << " " << endl;
    dat3.output();
    
    delete get<0>(data);
    delete get<1>(data);
    delete get<2>(data);
    
    return 0;
}

