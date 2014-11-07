#include <cstdlib>
#include <vector>

using namespace std;

#ifndef MATHFUNCTION_H_
#define MATHFUNCTION_H_

class mathFunction;
class creature;

class mathTerm
{
public:
	double outerCoef; //multiply the whole term by this
	double innerCoef; //multiply the x at the center of the expression by this. So if outerCoef=2, innerCoef=3, special=4, and power=5, then the expression is 2*(cos(3x)^4) 
	int specialFunc; //0 is none, 1 is exponential (e^x), 2 is ln, 3 is sin, 4 is cos
	double power; //puts the whole term to this power. i.e. say specialFunc=3, power=2. That's (sin(x))^2. If special func is 0 and power is .5, that's sqrt(x). Etc.
	mathFunction* actOn; //If null, this is x. Otherwise, this is a function which the above applies to. Say this represents x+1, power is -2, and special func is 0. Then this represents 1/((x+1)^2
	mathFunction* multBy; //multiplies the current term by this function. So if this is x, and specialFunc is 1 and power is 2, this does x*(e^x)^2
	mathTerm(double p_outerCoef=1.0, double p_innerCoef=1.0, double p_power=1.0, int p_specialFunc=0, mathFunction* p_actOn=NULL, mathFunction* p_multBy=NULL); //default is just x
	mathTerm(const mathTerm& source);
	double evaluate(double x); //plug in point xz
	~mathTerm();
};

class mathFunction
{
	friend mathFunction* geneMixer(mathFunction*, mathFunction*);
	friend class creature;
private:
	vector<mathTerm*> terms;
public:
	mathFunction();
	mathFunction(const mathFunction& source);
	mathFunction& operator=(const mathFunction& source);
	void add(mathTerm* t);
	double evaluate(double x);
	int intEval(int x);
	~mathFunction();
};

#endif /* MATHFUNCTION_H_ */
