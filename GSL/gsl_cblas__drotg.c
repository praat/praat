#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_drotg (double *a, double *b, double *c, double *s)
{
#define BASE double
#include "gsl_cblas__source_rotg.h"
#undef BASE
}
