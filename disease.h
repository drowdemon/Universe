#ifndef DISEASE_H
#define	DISEASE_H

#define WATER_TRANSMIT 1
#define FOOD_TRANSMIT 2
#define WASTE_TRANSMIT 4
#define CONTACT_TRANSMIT 8
#define ANIMAL_TRANSMIT 16

class disease
{
public:
    int energyCost; //how much energy you lose when you have this
    int strengthCost; //how much str you lose
    int intelCost; //intel loss
    int immunCost; //immunity loss
    int permStrCost; //permanent strength loss
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
    disease(int ec, int sc, int ic, int mr, int mm, int dur, int sa, int sprc, unsigned char t, int fc, int c, int tspr, int tsym, int ws, int f);
};

#endif	/* DISEASE_H */

