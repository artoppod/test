#include "iwarp.h"

iwarp_vals_t iwarp_vals =
{
	30,
	0.3,
	MOVE,
	false,
};

V<iwarpfloat, 2> deform_area_vectors[MAX_DEFORM_AREA_RADIUS * 2 + 1][MAX_DEFORM_AREA_RADIUS * 2 + 1];
iwarpfloat filter[MAX_DEFORM_AREA_RADIUS];
