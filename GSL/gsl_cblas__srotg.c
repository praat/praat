#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_srotg (float *a, float *b, float *c, float *s)
{
#define BASE float
#include "gsl_cblas__source_rotg.h"
#undef BASE
}
