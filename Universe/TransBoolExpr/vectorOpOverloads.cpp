#include "vectorOpOverloads.h"
#include <vector>
#include "../object.h"

using namespace std;

bool operator==(vector<object*> v, int i)
{
    for(unsigned int j=0; j<v.size(); j++)
    {
        if(v[j]->whatIsIt==i)
            return true;
    }
    return false;
}
bool operator==(vector<object*> v, vector<object*> v2)
{
    return false;
}
bool operator==(int i, vector<object*> v)
{
    for(unsigned int j=0; j<v.size(); j++)
    {
        if(v[j]->whatIsIt==i)
            return true;
    }
    return false;
}
bool operator>(vector<object*> v, int i)
{
    return (int)v.size()>i;
}
bool operator>(vector<object*> v, vector<object*> v2)
{
    return false;
}
bool operator>(int i, vector<object*> v)
{
    return i>(int)v.size();
}
bool operator>=(vector<object*> v, int i)
{
    return (int)v.size()>i;
}
bool operator>=(vector<object*> v, vector<object*> v2)
{
    return false;
}
bool operator>=(int i, vector<object*> v)
{
    return i>(int)v.size();
}
bool operator<(vector<object*> v, int i)
{
    return (int)v.size()<i;
}
bool operator<(vector<object*> v, vector<object*> v2)
{
    return false;
}
bool operator<(int i, vector<object*> v)
{
    return i<(int)v.size();
}
bool operator<=(vector<object*> v, int i)
{
    return (int)v.size()<=i;
}
bool operator<=(vector<object*> v, vector<object*> v2)
{
    return false;
}
bool operator<=(int i, vector<object*> v)
{
    return i<(int)v.size();
}
bool operator!=(vector<object*> v, int i)
{
    for(unsigned int j=0; j<v.size(); j++)
    {
        if(v[j]->whatIsIt==i)
            return false;
    }
    return true;
}
bool operator!=(vector<object*> v, vector<object*> v2)
{
    return false;
}
bool operator!=(int i, vector<object*> v)
{
    for(unsigned int j=0; j<v.size(); j++)
    {
        if(v[j]->whatIsIt==i)
            return false;
    }
    return true;
}