#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_daxpy (const int N, const double alpha, const double *X, const int incX,
             double *Y, const int incY)
{
#define BASE double
#include "gsl_cblas__source_axpy_r.h"
#undef BASE
}
