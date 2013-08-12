#ifndef OBJECT_H
#define	OBJECT_H

#define OBJECT_SMALLWOOD 0

class unit;
class hiveMind;

#define LISTVARSOBJVAR \
    Z(short, weight) \
    Z(short, whatIsIt)

#define LISTVARSOBJSTAT \
    Z(bool, sleepable) \
    Z(bool, walkable) 

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
    objectDescriptor(short w, short wv, short pid, bool walk, bool sleep);
};

class object
{
    friend class tile;
    friend class unit;
private:
    short weight;
    short heldByPlayer;
    short heldByIndex;
    short y;
    short x;
    short whatIsIt; //subject to change
    object(short w, short p, short i, short px, short py, short what);
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

