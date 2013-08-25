#include "boolList.h"
#include <vector>
#include <tuple>
#include <cstdlib>
#include <iostream>

using namespace std;

bool boolList::AND(bool a, bool b)
{
    return a&&b;
}
bool boolList::OR(bool a, bool b)
{
    return a||b;
}
bool (* boolList::ops[2])(bool, bool) = {boolList::AND, boolList::OR};
boolList::boolList()
{
}

tuple<int*,int*,int*> boolList::run(tuple<int*,int*,int*> data)
{
    vector<bool> res;
    for(unsigned int i=0; i<conds.size(); i++)
    {
        //res.push_back((conds[i].ops[conds[i].whatOp])(*(getIndVal(data,conds[i].whichvar)),((conds[i].valOrVar)?(conds[i].compVal):(*getIndVal(data, conds[i].compVar)))));
        res.push_back(helper(data,i));
        
    }
    for(unsigned int i=0; i<parens.size(); i++)
    {
        vector<bool> tempRes=res;
        vector<int> tempPerformOps=performOps;
        for(unsigned int j=0; j<parens[i].size(); j++)
        {
            vector<int> mappings;
            for(int k=0; k<res.size(); k++)
                mappings.push_back(k);
            tempRes[parens[i][j]]=(ops[performOps[parens[i][j]]])(res[parens[i][j]],res[parens[i][j]+1]);
            tempRes.erase(tempRes.begin()+parens[i][j]+1);
            tempPerformOps.erase(tempPerformOps.begin()+parens[i][j]);
            mappings[parens[i][j]+1]=parens[i][j];
        }
        res=tempRes;
        performOps=tempPerformOps;
    }
    if(res.size()!=1)
    {
        cout << "Error in boolList::run()!!!" << endl;
        exit(-54);
    }
    if(res[0]) //true, return requested info
    {
        return make_tuple((wantedData[0] ? get<0>(data) : NULL),(wantedData[1] ? get<1>(data) : NULL), (wantedData[2] ? get<2>(data) : NULL));
    }
    else
    {
        tuple<int*,int*,int*> empty;
        get<0>(empty)=NULL;
        get<1>(empty)=NULL;
        get<2>(empty)=NULL;        
        return empty;
    }
}

bool boolList::helper(tuple<int*,int*,int*> data, int condInd)
{
    switch(conds[condInd].whichvar)
    {
        case 0:
            return helper2(get<0>(data), data, condInd);
        case 1:
            return helper2(get<1>(data), data, condInd);
        case 2:
            return helper2(get<2>(data), data, condInd);
    }
    return 0;
}

template<class T>
bool boolList::helper2(T &datapoint, tuple<int*,int*,int*> data, int condInd)
{
    if(conds[condInd].valOrVar==false)
    {
        switch(conds[condInd].compVar)
        {
            case 0:
                return helper3(*datapoint,*(get<0>(data)),condInd);
            case 1:
                return helper3(*datapoint,*(get<1>(data)),condInd);
            case 2:
                return helper3(*datapoint,*(get<2>(data)),condInd);
        }
    }
    else
        return helper3(*datapoint,conds[condInd].compVal,condInd);
}

template<class T1, class T2>
bool boolList::helper3(T1& datapoint, T2& datapoint2, int condInd)
{
    switch(conds[condInd].whatOp)
    {
        case 0:
            return (datapoint==datapoint2);
        case 1:
            return (datapoint>datapoint2);
        case 2:
            return (datapoint>=datapoint2);
        case 3:
            return (datapoint<datapoint2);
        case 4:
            return (datapoint<=datapoint2);
        case 5:
            return (datapoint!=datapoint2);
    }
}