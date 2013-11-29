#include "boolListUnit.h"
#include <vector>
#include <tuple>
#include <cstdlib>
#include <iostream>
#include "vectorOpOverloads.h"

using namespace std;

bool boolListUnit::AND(bool a, bool b)
{
    return a&&b;
}
bool boolListUnit::OR(bool a, bool b)
{
    return a||b;
}
bool (* boolListUnit::ops[2])(bool, bool) = {boolListUnit::AND, boolListUnit::OR};
boolListUnit::boolListUnit()
{
}

unitTuple boolListUnit::run(unitTuple data)
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
            tempRes[parens[i][j]-j]=(ops[performOps[parens[i][j]]])(res[parens[i][j]],res[parens[i][j]+1]);
            tempRes.erase(tempRes.begin()+parens[i][j]+1-j);
            tempPerformOps.erase(tempPerformOps.begin()+parens[i][j]-j);
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
        return make_tuple((wantedData[0] ? get<0>(data) : NULL),(wantedData[1] ? get<1>(data) : NULL),(wantedData[2] ? get<2>(data) : NULL),(wantedData[3] ? get<3>(data) : NULL),(wantedData[4] ? get<4>(data) : NULL),(wantedData[5] ? get<5>(data) : NULL),(wantedData[6] ? get<6>(data) : NULL),(wantedData[7] ? get<7>(data) : NULL),(wantedData[8] ? get<8>(data) : NULL),(wantedData[9] ? get<9>(data) : NULL),(wantedData[10] ? get<10>(data) : NULL),(wantedData[11] ? get<11>(data) : NULL),(wantedData[12] ? get<12>(data) : NULL),(wantedData[13] ? get<13>(data) : NULL),(wantedData[14] ? get<14>(data) : NULL),(wantedData[15] ? get<15>(data) : NULL),(wantedData[16] ? get<16>(data) : NULL),(wantedData[17] ? get<17>(data) : NULL),(wantedData[18] ? get<18>(data) : NULL),(wantedData[19] ? get<19>(data) : NULL),(wantedData[20] ? get<20>(data) : NULL),(wantedData[21] ? get<21>(data) : NULL),(wantedData[21] ? get<22>(data) : NULL));
    }
    else
    {
        unitTuple empty;
        // <editor-fold defaultstate="collapsed" desc="Setting Empty To Null">
        get<0>(empty)=NULL;
        get<1>(empty)=NULL;
        get<2>(empty)=NULL;
        get<3>(empty)=NULL;
        get<4>(empty)=NULL;
        get<5>(empty)=NULL;
        get<6>(empty)=NULL;
        get<7>(empty)=NULL;
        get<8>(empty)=NULL;
        get<9>(empty)=NULL;
        get<10>(empty)=NULL;
        get<11>(empty)=NULL;
        get<12>(empty)=NULL;
        get<13>(empty)=NULL;
        get<14>(empty)=NULL;
        get<15>(empty)=NULL;
        get<16>(empty)=NULL;
        get<17>(empty)=NULL;
        get<18>(empty)=NULL;
        get<19>(empty)=NULL;
        get<20>(empty)=NULL;
        get<21>(empty)=NULL;
        get<22>(empty)=NULL;
        // </editor-fold>
        return empty;
    }
}

bool boolListUnit::helper(unitTuple data, int condInd)
{
    switch(conds[condInd].whichvar)
    {
        // <editor-fold defaultstate="collapsed" desc="case i: return helper2(get<i>(data), data, condInd);">
        case 0:
            return helper2(get<0>(data), data, condInd);
        case 1:
            return helper2(get<1>(data), data, condInd);
        case 2:
            return helper2(get<2>(data), data, condInd);
        case 3:
            return helper2(get<3>(data), data, condInd);
        case 4:
            return helper2(get<4>(data), data, condInd);
        case 5:
            return helper2(get<5>(data), data, condInd);
        case 6:
            return helper2(get<6>(data), data, condInd);
        case 7:
            return helper2(get<7>(data), data, condInd);
        case 8:
            return helper2(get<8>(data), data, condInd);
        case 9:
            return helper2(get<9>(data), data, condInd);
        case 10:
            return helper2(get<10>(data), data, condInd);
        case 11:
            return helper2(get<11>(data), data, condInd);
        case 12:
            return helper2(get<12>(data), data, condInd);
        case 13:
            return helper2(get<13>(data), data, condInd);
        case 14:
            return helper2(get<14>(data), data, condInd);
        case 15:
            return helper2(get<15>(data), data, condInd);
        case 16:
            return helper2(get<16>(data), data, condInd);
        case 17:
            return helper2(get<17>(data), data, condInd);
        case 18:
            return helper2(get<18>(data), data, condInd);
        case 19:
            return helper2(get<19>(data), data, condInd);
        case 20:
            return helper2(get<20>(data), data, condInd);
        case 21:
            return helper2(get<21>(data), data, condInd);
        case 22:
            return helper2(get<22>(data), data, condInd);
            // </editor-fold>
    }
    cout << "Error in boolList::helper" << endl;
    exit(-9);
    return 0;
}

template<class T>
bool boolListUnit::helper2(T &datapoint, unitTuple data, int condInd)
{
    if(conds[condInd].valOrVar==false)
    {
        switch(conds[condInd].compVar)
        {
            // <editor-fold defaultstate="collapsed" desc="case i: return helper3(*datapoint,*(get<0>(data)),condInd);">
            case 0:
                return helper3(*datapoint,*(get<0>(data)),condInd);
            case 1:
                return helper3(*datapoint,*(get<1>(data)),condInd);
            case 2:
                return helper3(*datapoint,*(get<2>(data)),condInd);
            case 3:
                return helper3(*datapoint,*(get<3>(data)),condInd);
            case 4:
                return helper3(*datapoint,*(get<4>(data)),condInd);
            case 5:
                return helper3(*datapoint,*(get<5>(data)),condInd);
            case 6:
                return helper3(*datapoint,*(get<6>(data)),condInd);
            case 7:
                return helper3(*datapoint,*(get<7>(data)),condInd);
            case 8:
                return helper3(*datapoint,*(get<8>(data)),condInd);
            case 9:
                return helper3(*datapoint,*(get<9>(data)),condInd);
            case 10:
                return helper3(*datapoint,*(get<10>(data)),condInd);
            case 11:
                return helper3(*datapoint,*(get<11>(data)),condInd);
            case 12:
                return helper3(*datapoint,*(get<12>(data)),condInd);
            case 13:
                return helper3(*datapoint,*(get<13>(data)),condInd);
            case 14:
                return helper3(*datapoint,*(get<14>(data)),condInd);
            case 15:
                return helper3(*datapoint,*(get<15>(data)),condInd);
            case 16:
                return helper3(*datapoint,*(get<16>(data)),condInd);
            case 17:
                return helper3(*datapoint,*(get<17>(data)),condInd);
            case 18:
                return helper3(*datapoint,*(get<18>(data)),condInd);
            case 19:
                return helper3(*datapoint,*(get<19>(data)),condInd);
            case 20:
                return helper3(*datapoint,*(get<20>(data)),condInd);
            case 21:
                return helper3(*datapoint,*(get<21>(data)),condInd);
            case 22:
                return helper3(*datapoint,*(get<22>(data)),condInd);
            //</editor-fold>
        }
    }
    else
        return helper3(*datapoint,conds[condInd].compVal,condInd);
    cout << "Error in boolList::helper2" << endl;
    exit(-99);
    return false;
}

template<class T1, class T2>
bool boolListUnit::helper3(T1& datapoint, T2& datapoint2, int condInd)
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
    cout << "Error in boolList::helper3" << endl;
    exit(-999);
    return false;
}