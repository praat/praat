#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

float
cblas_scasum (const int N, const void *X, const int incX)
{
#define BASE float
#include "gsl_cblas__source_asum_c.h"
#undef BASE
}
