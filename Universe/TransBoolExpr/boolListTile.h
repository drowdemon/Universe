#ifndef BOOLLISTTILE_H
#define	BOOLLISTTILE_H

#include "boolCond.h"
#include <vector>
#include <tuple>

using std::vector;
using std::tuple;

#define ANDIND 0
#define ORIND 1

class object;

typedef tuple<unsigned char*, unsigned short*, short*, unsigned char*, unsigned short*, unsigned char*, unsigned char*, short*, short*, bool*, short*, short*, vector<object*>*> tileTuple;

class boolListTile
{
public:
    vector<bool> wantedData; //The data you want, for each index in the tuple. So its size is equal to the tuple's size.
    vector<boolCond> conds; //The conditions, in terms of given tuple
    vector<vector<int> > parens;  //Where the parentheses are. First vector is where the int given is the first index, and then the next index is the final one in the parentheses. Next the parens where there is a larger gap, by 1.  There are no precedence conditions. Not even left to right. This means that in any group of 3 operations, 2 must be in parentheses no matter what. And there must be all enclosing parens. So if there is a list of 4 bools, X,X,X,X, There must be something like ((X,X)(X,X)). (X,X),X,X is NOT allowed. X,X is NOT allowed. (X,X) is. 
    vector<int> performOps; // the operations to preform to combine conditions, in order from left to right. So the size of this should be one less than the size of conds. The zeroth one is between the things at index 0 and 1. Also, this must contain a 0 or a 1. 
    static bool OR(bool a, bool b);
    static bool AND(bool a, bool b);
    static bool (*ops[2])(bool, bool); //0=AND, 1=OR
    tileTuple run(tileTuple data);
    bool helper(tileTuple data, int condInd);
    
    template<class T>
    bool helper2(T &datapoint, tileTuple data, int condInd);
    template<class T1, class T2>
    bool helper3(T1 &datapoint, T2 &datapoint2, int condInd);
    boolListTile();
};

#endif	/* BOOLLISTTILE_H */

