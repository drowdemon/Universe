#include "disease.h"

#define X(type, var) type p_ ## var,
disease::disease(LISTVARSDISEASE bool extraneous)
{
#undef X
#define X(type, var) var = p_ ## var ;
	LISTVARSDISEASE
#undef X
}
