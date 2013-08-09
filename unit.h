#ifndef UNIT_H
#define	UNIT_H

class unit
{
public: 
	//more will certainly be added. Our own creatures will inherit from this, and implement some sort of run function, probably
	int player;
	int index; //in whatever list of units there will be
	short sleep; //how well-rested it is. At some point sleep deprivation starts taking effect
	short energy; //how much energy it has. Replenished by eating. Taken up by living, moving, fighting, etc.
	short strength; //affects how much it can carry, how well it can fight, etc.
	bool gender; //I assume 2 genders
	short intelligence; //effects various tasks. 
	unsigned char age;
	short diseased; //0 if no, else index in something or another of the disease
	short timeDiseased; //how long it has been diseased, i.e. how far along the progression of the disease has gone
	short x;
	short y;
        short speed; //how often movement occurs. Can't be 1:1 with ticks, since vehicles have to be faster
        short lineOfSight; //how far it can see. Elevation increases? Buildings/obstacles block sight
        
        //below variables are not passed to constructor
        short moveToX;
        short moveToY;
	unit(int p, int i, short s, short e, short str, bool g, short intel, unsigned char a, short d, short td, short px, short py, short pspeed, short los);
	void nextFrame();
        void move(); //no obstacle avoidance: each creature will implement that on its own. This just moves in the direction of a target. Very simple.
};

#endif	/* UNIT_H */

