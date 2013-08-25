#include <tuple>
#include "boolCond.h"

using namespace std;
/*
template<class T1, class T2>
bool boolCond::Eq(T1 a, T2 b)
{
    return a==b;
}
template<class T1, class T2>
bool boolCond::Greater(T1 a, T2 b)
{
    return a>b;
}
template<class T1, class T2>
bool boolCond::GreaterEq(T1 a, T2 b)
{
    return a>=b;
}
template<class T1, class T2>
bool boolCond::Less(T1 a, T2 b)
{
    return a<b;
}
template<class T1, class T2>
bool boolCond::LessEq(T1 a, T2 b)
{
    return a<=b;
}
template<class T1, class T2>
bool boolCond::NEq(T1 a, T2 b)
{
    return a!=b;
}

bool (* boolCond::ops[6])(int, int) = {boolCond::Eq,boolCond::Greater, boolCond::GreaterEq, boolCond::Less, boolCond::LessEq, boolCond::NEq};*/

boolCond::boolCond(int whichV, int compVl, int compVr, bool vlOrVr, int wo)
{
    whichvar=whichV;
    compVal=compVl;
    compVar=compVr;
    valOrVar=vlOrVr;
    whatOp=wo;
}