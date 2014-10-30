#include "mathFunction.h"

#include <cmath>

mathTerm::mathTerm(double p_outerCoef, double p_innerCoef, double p_power, int p_specialFunc, mathFunction* p_actOn, mathFunction* p_multBy)
{
	outerCoef=p_outerCoef;
	innerCoef=p_innerCoef;
	power=p_power;
	specialFunc=p_specialFunc;
	actOn=p_actOn;
	multBy=p_multBy;
}

mathTerm::~mathTerm()
{
	if(actOn)
	{
		delete actOn;
		actOn=NULL;
	}
	if(multBy)
	{
		delete multBy;
		multBy = NULL;
	}
}

double mathTerm::evaluate(double x)
{
	double ret = innerCoef;
	if(actOn)
		ret *= actOn->evaluate(x);
	else
		ret *= x;
	switch(specialFunc)
	{
	case 0:
		break;
	case 1:
		ret = exp(ret);
		break;
	case 2:
		ret = log(ret);
		break;
	case 3:
		ret = sin(ret);
		break;
	case 4:
		ret = cos(ret);
		break;
	}
	ret = outerCoef * pow(ret, power);
	if(multBy)
		ret *= multBy->evaluate(x);
	return ret;
}

mathFunction::~mathFunction()
{
	for(unsigned int i=0; i<terms.size(); i++)
	{
		if(terms[i])
		{
			delete terms[i];
			terms[i]=NULL;
		}
	}
}

double mathFunction::evaluate(double x)
{
	double sum = 0;
	for(unsigned int i=0; i<terms.size(); i++)
	{
		sum += terms[i]->evaluate(x);
	}
	return sum;
}

void mathFunction::add(mathTerm *t)
{
	terms.push_back(t);
}
