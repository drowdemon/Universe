#ifndef SPECIES_H
#define	SPECIES_H

class animalEatingAbilities;

class species
{
public:
    short health;
    short speed;
    short lineOfSight;
    short animalType;
    animalEatingAbilities *eatingBehavior;
    species(short h, short s, short los, short at, animalEatingAbilities *eb);
};

#endif	/* SPECIES_H */

