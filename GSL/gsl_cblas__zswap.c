#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_zswap (const int N, void *X, const int incX, void *Y, const int incY)
{
#define BASE double
#include "gsl_cblas__source_swap_c.h"
#undef BASE
}
