#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_drotmg (double *d1, double *d2, double *b1, const double b2, double *P)
{
#define BASE double
#include "gsl_cblas__source_rotmg.h"
#undef BASE
}
