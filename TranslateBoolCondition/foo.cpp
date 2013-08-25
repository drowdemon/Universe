#include "foo.h"
#include <iostream>
#include <string>

using namespace std;

foo::foo()
{
    a=b=0;
}

foo::foo(int pa, int pb)
{
    a=pa;
    b=pb;
}

bool operator==(foo f, int n)
{
    return (f.a+f.b)==n;
}
bool operator==(int n, foo f)
{
    return (f.a+f.b)==n;
}
bool operator>(foo f, int n)
{
    return (f.a+f.b)>n;
}
bool operator>=(foo f, int n)
{
    return (f.a+f.b)>=n;
}
bool operator<(foo f, int n)
{
    return (f.a+f.b)<n;
}
bool operator<=(foo f, int n)
{
    return (f.a+f.b)<=n;
}
bool operator!=(foo f, int n)
{
    return (f.a+f.b)!=n;
}
bool operator>(int n, foo f)
{
    return n>(f.a+f.b);
}
bool operator>=(int n, foo f)
{
    return n>=(f.a+f.b);
}
bool operator<(int n, foo f)
{
    return n<(f.a+f.b);
}
bool operator<=(int n, foo f)
{
    return n<=(f.a+f.b);
}
bool operator!=(int n, foo f)
{
    return (f.a+f.b)!=n;
}
bool operator==(foo f, foo f2)
{
    return (f.a+f.b)==(f2.a+f2.b);
}
bool operator>(foo f, foo f2)
{
    return (f.a+f.b)>(f2.a+f2.b);
}
bool operator>=(foo f, foo f2)
{
    return (f.a+f.b)>=(f2.a+f2.b);
}
bool operator<(foo f, foo f2)
{
    return (f.a+f.b)<(f2.a+f2.b);
}
bool operator<=(foo f, foo f2)
{
    return (f.a+f.b)<=(f2.a+f2.b);
}
bool operator!=(foo f, foo f2)
{
    return (f.a+f.b)!=(f2.a+f2.b);
}

void foo::output()
{
    cout << "foo::a: " << a << endl;
    cout << "foo::b: " << b << endl;
}