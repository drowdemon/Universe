#ifndef COMMWRAPPER_H
#define	COMMWRAPPER_H

#include "commProtocol.h"
#include <vector>

using std::vector;

class commWrapper
{
    friend void reformat();
private:
    vector<communicationProtocol> communiques;
    int player; //The player and index of the player that owns this
    int index; //index updating handled in reformat()
public:
    commWrapper();
    void push(communicationProtocol &cp, unit *u);
    communicationProtocol read(unit *u, int index);
    void erase(unit *u, int index);
    void popBack(unit *u);
};

#endif	/* COMMWRAPPER_H */

