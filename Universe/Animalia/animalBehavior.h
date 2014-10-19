#ifndef ANIMALBEHAVIOR_H
#define	ANIMALBEHAVIOR_H

class animalBehavior : animal
{
public:
    virtual void behavior() = 0;
    virtual ~animalBehavior();
};

#endif	/* ANIMALBEHAVIOR_H */

