#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

double
cblas_dznrm2 (const int N, const void *X, const int incX)
{
#define BASE double
#include "gsl_cblas__source_nrm2_c.h"
#undef BASE
}
