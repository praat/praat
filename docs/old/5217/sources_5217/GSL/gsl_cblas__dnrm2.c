#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

double
cblas_dnrm2 (const int N, const double *X, const int incX)
{
#define BASE double
#include "gsl_cblas__source_nrm2_r.h"
#undef BASE
}
