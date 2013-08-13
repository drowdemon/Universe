#ifndef FOOD_H
#define	FOOD_H

#include <vector>

using std::vector;

class food
{
    friend class unit;
    friend void init();
    friend class object;
    friend class objectDescriptor;
    friend int main();
private:
    short timeWhileGood; //how long until rot begins
    short rotSeverity; //increases over time, eventually reaches a max and it ceases to be
    short nutrition; //hunger reduced when consumed
    short origNutrition;
    short goodTimeVariation;
    food(short twg=-1, short rs=-1, short n=-1, short gtv=-1, short origN=-1);
    vector<int> rot();
    bool operator!();
};

#endif	/* FOOD_H */

