/* 
 * File:   foo.h
 * Author: marc
 *
 * Created on August 24, 2013, 11:16 PM
 */

#ifndef FOO_H
#define	FOO_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class foo
{
    int a;
    int b;
    foo();
    friend class vector<foo>;
public:
    foo(int pa, int pb);
    void output();
    friend bool operator==(int, foo);
    friend bool operator==(foo, int);
    friend bool operator>(foo, int);
    friend bool operator>=(foo, int);
    friend bool operator<(foo, int);
    friend bool operator<=(foo, int);
    friend bool operator!=(foo, int);
    friend bool operator>(int, foo);
    friend bool operator>=(int, foo);
    friend bool operator<(int, foo);
    friend bool operator<=(int, foo);
    friend bool operator!=(int, foo);
    friend bool operator==(foo, foo);
    friend bool operator>(foo, foo);
    friend bool operator>=(foo, foo);
    friend bool operator<(foo, foo);
    friend bool operator<=(foo, foo);
    friend bool operator!=(foo, foo);    
};

bool operator==(int, foo);
bool operator==(foo, int);
bool operator>(foo, int);
bool operator>=(foo, int);
bool operator<(foo, int);
bool operator<=(foo, int);
bool operator!=(foo, int);
bool operator>(int, foo);
bool operator>=(int, foo);
bool operator<(int, foo);
bool operator<=(int, foo);
bool operator!=(int, foo);
bool operator==(foo, foo);
bool operator>(foo, foo);
bool operator>=(foo, foo);
bool operator<(foo, foo);
bool operator>=(foo, foo);
bool operator!=(foo, foo);

#endif	/* FOO_H */

