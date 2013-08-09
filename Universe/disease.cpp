#include "disease.h"

disease::disease(int ec, int sc, int ic, int mr, int mm, int dur, int sa, int sprc, unsigned char t, int fc, int c, int tspr, int tsym, int ws, int f)
{
    energyCost=ec;
    strengthCost=sc;
    intelCost=ic;
    multiplierRate=mr;
    multiplierMax=mm;
    duration=dur;
    spreadabilityArea=sa;
    spreadabilityChance=sprc;
    transmit=t;
    first=f;
    firstChance=fc;
    curability=c;
    timeForSpreadability=tspr;
    timeForSymptoms=tsym;
    waterSpreadability=ws;
}
