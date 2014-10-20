#ifndef SPECIES_H_
#define SPECIES_H_

#define LISTVARSSPECIES \
	X(short, maxHealth) \
	X(short, speed) \
	X(short, lineOfSight) \
	X(short, animalType) \
	X(short, movementEnergy) \
	X(short, fallingMultiplier) \
	X(short, maxHunger) \
	X(short, energyCritPoint) \
	X(short, gestationPeriod) \
	X(short, pregnantEnergyCost) \
	X(short, reproductionEnergyCost) \
	X(short, newbornEnergy) \
	X(short, newbornSleep) \
	X(short, newbornHunger) \
	X(short, birthHealthLoss) \
	X(short, birthEnergyLoss) \
	X(short, reproductionTime) \
	X(short, energySoftMax) \
	X(short, energyFromFatPoint) \
	X(short, energyFromFatRate) \
	X(short, newbornMinWeight) \
	X(short, movingSelfWeightPenalty) 

class animalEatingAbilities;

class species //species 0 is the player
{
private:
	friend class animal;
	friend class creature;
	friend void init();
	
	short maxHealth;
	short speed;
	short lineOfSight;
	short animalType;
	
	short movementEnergy;
	short fallingMultiplier; //DIVIDE BY 100 TO USE when you fall, the distance fallen * this = damage done. 
	short maxHunger;
	short energyCritPoint; //if energy falls below this, maxmetabolicrate is used
	short gestationPeriod; //arbitrary, measured in system ticks. No clue how long this is
	short pregnantEnergyCost; //every 1000 units of time pregnant, this is multiplied. So 3,6,9,12,15
	short reproductionEnergyCost;
	short newbornEnergy; //I really don't know how much this is
	short newbornSleep;  
	short newbornHunger;
	short birthHealthLoss;
	short birthEnergyLoss;
	short reproductionTime;
    short energySoftMax; //arbitrary, as usual. After this, energy is converted to fat
    short energyFromFatPoint; //totally arbitrary. After this (below this), fat is converted to energy. Hunger still increases though.
    short energyFromFatRate; //again arbitrary. In frames per event. Weight transforms to fat at this rate.
    short newbornMinWeight; //how light a newborncan be, for constructor. After this, the min weight changes (deterministically) based on age, so it's a member variable.
    short movingSelfWeightPenalty; //per 50 weight, with it ceiled or floored according to rand()%(weight%50)
    
    animalEatingAbilities *eatingBehavior;
#define X(type, var) \
    type p_ ## var,
    species(LISTVARSSPECIES animalEatingAbilities *p_eatingBehavior);
#undef X
};

#endif /* SPECIES_H_ */
