#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

CBLAS_INDEX
cblas_idamax (const int N, const double *X, const int incX)
{
#define BASE double
#include "gsl_cblas__source_iamax_r.h"
#undef BASE
}
