#ifndef THROW_H
#define	THROW_H

#include <vector>

using std::vector;

class object;
class unit;

class Throwing
{
    friend class unit;
    friend class object;
private:
    int experience;
    const static int xpDiffToLearn;
    const static int maxLearnDistSquared;
    
    //formula of the form Speed=BaseSpeed+Strength*StrengthToSpeed/100+exp*xpToSpeed/100+Weight*WeightSpeedSubt/100. Same for rest.    
    //This determines how fast an object will go. Every frame, the object slows down according to SpeedDecPerWeight*Weight. The object itself may be aerodynamic or not, and so can have an additional constant modifier. That modifier times the current speed over some power of 10 constant will be subtracted from the speed. Aerodynamic objects will have a negative constant.
    //The speed will also be affected by the direction the object is traveling in. As the object is traveling, if height goes from high to low the object will speed up (i.e. go farther) and the reverse.
    const static int StrengthToSpeed;
    const static int WeightSpeedSubt;
    const static int xpToSpeed;
    const static int BaseSpeed;
    const static int SpeedDecPerWeight;
    const static int HeightDiffSpeedChange; //increase or decrease as object rises or falls, per unit height. In our world, this is inaccurate, but since this world is '2.5D' it will serve. It is possible for the height to increase so much that the object stops entirely. [This should be applied before the object hits its target]. That will be how walls work.
    
    vector<int> accuracyX; //size MUST be odd. The central element is the square that the unit is aiming for. Each element to the left, is one more to the left of that square, etc. Each element of the vector contains the probability of hitting that square, out of a total of 1000. i.e. The sum of the numbers in the vector must add to 1000
    vector<int> accuracyY; //Same for Y. Updated in learn. 
    //accuracyZ is implicit. If you aim high, then you end up going farther in y. Low is closer in y.
    const static int MovingAccuracyReduction; //penalty in accuracy for throwing while moving
    
    int accStageX; //for learning
    int accStageY; //for learning
    const static int xpToAccuracy;
    bool threw; //when someone throws, this is active until the next iteration that includes that unit. Thus, learning is possible.
    int numSeen; //for learning: num seen. Once reaches particular value, there will be xp gain. Used when learning from someone else.
    const static int seenToXp;
    
    
    Throwing(int e);
    void Throw(int objIndex, unit *who, int x, int y, bool moving);
    void learn(); //adds 1 to xp, changes accuracy if necessary
    void learn(unit *student, unit *teacher); //learning from someone. Calls the simpler learn solo if xp is added.
    void tempUnLearn(vector<int> &accX, vector<int> &accY, unsigned int qty); //when accuracy is temporarily reduced for whatever reason, this is called.
};

#endif	/* THROW_H */

