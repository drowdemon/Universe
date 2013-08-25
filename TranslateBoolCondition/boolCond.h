#ifndef BOOLCOND_H
#define	BOOLCOND_H

#define EQ 0
#define GREATER 1
#define GREATEREQ 2
#define LESS 3
#define LESSEQ 4
#define NEQ 5

class boolCond
{
public:
    int whichvar; //which var in the tuple will be a
    int compVal; //what the value of b is
    int compVar; //which var in the tuple will be b
    bool valOrVar; //true=compval, false=compvar. Which one of them is really b.
    int whatOp; //which operation to use
    /*template<class T1, class T2>
    static bool Less(T1 a, T2 b); //operations
    template<class T1, class T2>
    static bool LessEq(T1 a, T2 b); //operations
    template<class T1, class T2>
    static bool Eq(T1 a, T2 b); //operations
    template<class T1, class T2>
    static bool NEq(T1 a, T2 b); //operations
    template<class T1, class T2>
    static bool Greater(T1 a, T2 b); //operations
    template<class T1, class T2>
    static bool GreaterEq(T1 a, T2 b); //operations
    static bool (*ops[6])(int, int); */
    boolCond(int whichV, int compVl, int compVr, bool vlOrVr, int wo);
    
};

#endif	/* BOOLCOND_H */

