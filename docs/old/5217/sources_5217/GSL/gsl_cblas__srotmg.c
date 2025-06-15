#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_srotmg (float *d1, float *d2, float *b1, const float b2, float *P)
{
#define BASE float
#include "gsl_cblas__source_rotmg.h"
#undef BASE
}
