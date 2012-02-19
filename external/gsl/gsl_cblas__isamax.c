#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

CBLAS_INDEX
cblas_isamax (const int N, const float *X, const int incX)
{
#define BASE float
#include "gsl_cblas__source_iamax_r.h"
#undef BASE
}
