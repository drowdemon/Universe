#include "dataStructures.h"

diseaseInfo::diseaseInfo(int d, int t, int m)
{
    disease=d;
    time=t;
    multiplier=m;
    flipDir=false;
}
diseaseInfo::operator  int()
{
    return disease;
}