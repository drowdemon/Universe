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
    unsigned short rotRate; //rate at which it rots, in frames. Applies after timeWhileGood passes. Musn't be 0
    short nutrition; //hunger reduced when consumed
    unsigned short origNutrition; //what the nutrition was when created. //unsigned because it's >=0, not because it needs to be big
    short goodTimeVariation;
    bool vegetarian;
    food(short twg=-1, short rs=-1, unsigned short rr=0, short n=-1, short gtv=-1, unsigned short origN=0, bool veg=false);
    vector<int> rot(); //returns a vector of the indices in all diseases of the diseases this piece of food will transmit due to rotting.
public:
    bool getVegetarian();
};

#endif	/* FOOD_H */

