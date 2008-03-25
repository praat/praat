#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_sswap (const int N, float *X, const int incX, float *Y, const int incY)
{
#define BASE float
#include "gsl_cblas__source_swap_r.h"
#undef BASE
}
