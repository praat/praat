#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

double
cblas_dzasum (const int N, const void *X, const int incX)
{
#define BASE double
#include "gsl_cblas__source_asum_c.h"
#undef BASE
}
