#ifndef OBJECT_H
#define	OBJECT_H

#include <vector>
#include "food.h"

using std::vector;

#define OBJECT_SMALLWOOD 0
#define OBJECT_CORPSE 1

class unit;
class hiveMind;

#define LISTVARSOBJVAR \
    Z(short, weight) \
    Z(short, whatIsIt) \

#define LISTVARSOBJSTAT \
    Z(bool, sleepable) \
    Z(bool, walkable) \
    Z(bool, vaguelyEdible)

class objectDescriptor
{
    friend class tile;
private:
    friend class object;
    friend void init();
    short weight;
    short weightVariation;
    short id;
    bool walkable;
    bool sleepable;
    bool vaguelyEdible; //false=no. Like a piece of wood. Or a rock. NOT edible. true=vaguely.
    short actuallyEdible; //-3=no. Like above, but private. -2=no. Poisonous (infected with a disease, that will naturally go away in -1 time. Includes raw meat). -1=no, but not poisonous. Like grass. It just goes straight through the system. 0 or above is yes, and points to the index of the food on the allfoods vector of foods that will probably end up existing. 
    short disease; //if its poisonous, it will have a disease. Other diseases may exist, but not by default.
    food possFood;
    objectDescriptor(short w, short wv, short pid, bool walk, bool sleep, bool vedib, short aedib, short dis, food f);
};

class object
{
    friend class tile;
    friend class unit;
    friend class food;
    friend class Throwing;
    friend int main();
    friend void reformat();
    friend bool operator==(vector<object*> v, int i);
    friend bool operator==(int i, vector<object*> v);
    friend bool operator!=(vector<object*> v, int i);
    friend bool operator!=(int i, vector<object*> v);
private:
    short weight;
    short heldByPlayer;
    short heldByIndex;
    double y;
    double x;
    short whatIsIt; //subject to change
    bool actuallyEdible; //will change as rotting occurs
    vector<int> infected;
    vector<short> infectionTime;
    food possFood;
    short speed; //divide by 1000 and floor + (rand()%1000)<(decimal part*1000)?1:0).
    short toX; //where a moving object is going. Euclidean direction.
    short toY;
    short index; //in a tiles allobjects or a units carrying, whichever is applicable.
    short height;
    object(short w, short p, short i, short px, short py, short what, bool aedib, food pf, short ind, short h);
    object(objectDescriptor& od, short p, short i, short px, short py, short ind, short h);
    bool rot();
    void move();
public:
    //short getWeight(unit *u);
#define Z(type, val) \
    type get ## val(unit *u);
    LISTVARSOBJVAR
    LISTVARSOBJSTAT
#undef Z
#define Z(type, val) \
    type get ## val(hiveMind *h, short wx, short wy);
    LISTVARSOBJVAR
    LISTVARSOBJSTAT
#undef Z
#define Z(type, val) \
    type get ## val(hiveMind *h, short unitIndex);
    LISTVARSOBJVAR
    LISTVARSOBJSTAT
#undef Z
};

#endif	/* OBJECT_H */

