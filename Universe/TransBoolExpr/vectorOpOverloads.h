/* 
 * File:   vectorOpOverloads.h
 * Author: marc
 *
 * Created on August 25, 2013, 5:07 PM
 */

#ifndef VECTOROPOVERLOADS_H
#define	VECTOROPOVERLOADS_H

#include <vector>
class object;

using std::vector;

bool operator==(vector<object*> v, int i);
bool operator==(vector<object*> v, vector<object*> v2);
bool operator==(int i, vector<object*> v);
bool operator>(vector<object*> v, int i);
bool operator>(vector<object*> v, vector<object*> v2);
bool operator>(int i, vector<object*> v);
bool operator>=(vector<object*> v, int i);
bool operator>=(vector<object*> v, vector<object*> v2);
bool operator>=(int i, vector<object*> v);
bool operator<(vector<object*> v, int i);
bool operator<(vector<object*> v, vector<object*> v2);
bool operator<(int i, vector<object*> v);
bool operator<=(vector<object*> v, int i);
bool operator<=(vector<object*> v, vector<object*> v2);
bool operator<=(int i, vector<object*> v);
bool operator!=(vector<object*> v, int i);
bool operator!=(vector<object*> v, vector<object*> v2);
bool operator!=(int i, vector<object*> v);


#endif	/* VECTOROPOVERLOADS_H */

