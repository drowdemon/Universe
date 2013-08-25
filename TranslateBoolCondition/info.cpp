#include "info.h"
#include <tuple>

using namespace std;

info::info(int pa, int pb, int pc)
{
    a=pa;
    b=pb;
    c=pc;
    data = make_tuple(&a,&b,&c);
}

info::info()
{
    a=b=c=0;
    data = make_tuple(&a,&b,&c);
}