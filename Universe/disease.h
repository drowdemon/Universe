#ifndef DISEASE_H
#define	DISEASE_H

#define WATER_TRANSMIT 1
#define FOOD_TRANSMIT 2
#define WASTE_TRANSMIT 4
#define CONTACT_TRANSMIT 8     //contact transmit is animal-animal or unit-unit
#define ANIMAL_TRANSMIT 16     //animal transmit is animal-unit/unit-animal
#define ROTTEN_FOOD_TRANSMIT 32
	
#define LISTVARSDISEASE 			\
	X(int, energyCost) 				\
	X(int, strengthCost)				\
	X(int, intelCost)				\
	X(int, immunCost)				\
	X(double, permStrCost)			\
	X(int, permIntelCost)			\
	X(int, permImmunCost)			\
	X(int, multiplierRate)			\
	X(int, multiplierMax)			\
	X(int, duration)				\
	X(int, spreadabilityArea)		\
	X(int, spreadabilityChance) 	\
	X(unsigned char, transmit)   	\
	X(int, firstChance)				\
	X(int, curability)				\
	X(int, timeForSpreadability)	\
	X(int, timeForSymptoms)			\
	X(int, timeOnMap)				\
	X(int, waterSpreadability)  	\
	X(int, first)					\
	
class disease
{
    friend class tile;
    friend class unit;
    friend class creature;
    friend class food;
    friend class object;
private:
    int energyCost; //how much energy you lose when you have this
    int strengthCost; //how much str you lose   
    int intelCost; //intel loss
    int immunCost; //immunity loss
    double permStrCost; //permanent strength loss       
    int permIntelCost; //permanent intelligence loss
    int permImmunCost; //permanent immunity loss
    int multiplierRate; //As time passes, every this often the disease gets worse. Once it reaches the max, if the creature lives, it starts getting better. A disease kills by having an unsustainable energy cost.
    int multiplierMax; //This is the max multiplier. If this is reached, the disease starts getting better
    int duration;
    int spreadabilityArea;
    int spreadabilityChance; //ratio out of 10,000
    unsigned char transmit; //uses defined values. Can be |d together
    int firstChance; //ratio out of 10,000
    int curability; //out of 10,000
    int timeForSpreadability;
    int timeForSymptoms;
    int timeOnMap; //how long it can remain on the map, i.e. in water or in waste
    int waterSpreadability; //spreadability within bodies of water, out of 10,000
    
    int first; //0 if it doesn't matter whether this is the first time. I.e. raw meat, waste. 1 If it matters and it is the first time. 2 If it matters and it is not the first time. First time meaning no one has had it before, so there's some chance of someone getting it at random.
    
#define X(type, var) type p_ ## var, 
    disease(LISTVARSDISEASE bool extraneous=true);
#undef X
};

#endif	/* DISEASE_H */

