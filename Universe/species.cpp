#include "species.h"

#define X(type, var) \
	type p_ ## var, 
species::species(LISTVARSSPECIES animalEatingAbilities *p_eatingBehavior)
{
#undef X
#define X(type, var) \
	var = p_ ## var;
	LISTVARSSPECIES
#undef X
	eatingBehavior=p_eatingBehavior;
}
