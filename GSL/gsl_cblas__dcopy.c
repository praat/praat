#include "gsl_math.h"
#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_dcopy (const int N, const double *X, const int incX, double *Y,
             const int incY)
{
#define BASE double
#include "gsl_cblas__source_copy_r.h"
#undef BASE
}
