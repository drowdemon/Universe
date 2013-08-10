#ifndef ALLMINDS_H
#define	ALLMINDS_H

#include <vector>
#include "hivemind.h"

using std::vector;

class unit;
class hiveMind;

class allminds
{
    friend int main();
    friend class unit;
private:
    vector<vector<hiveMind> > data;
public:
    allminds();
    hiveMind* get(unit* u, int index);
    hiveMind* get(hiveMind* h, int index);
};

#endif	/* ALLMINDS_H */

