#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_zaxpy (const int N, const void *alpha, const void *X, const int incX,
             void *Y, const int incY)
{
#define BASE double
#include "gsl_cblas__source_axpy_c.h"
#undef BASE
}
