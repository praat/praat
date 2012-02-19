#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dswap (const int N, double *X, const int incX, double *Y,
             const int incY)
{
#define BASE double
#include "gsl_cblas__source_swap_r.h"
#undef BASE
}
